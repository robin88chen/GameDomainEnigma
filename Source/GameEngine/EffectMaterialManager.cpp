#include "EffectMaterialManager.h"
#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include "EffectCompiler.h"
#include "EffectResponses.h"
#include "EngineErrors.h"
#include "EffectRequests.h"
#include "EffectMaterialDto.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Platforms/PlatformLayer.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, EffectMaterialManager, ISystemService);

EffectMaterialManager::EffectMaterialManager(Frameworks::ServiceManager* srv_mngr,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer) : ISystemService(srv_mngr), m_currentCompilingRuid()
{
    m_effectDeserializer = effect_deserializer;
    m_needTick = false;
    m_compiler = menew EffectCompiler(this);
    m_isCurrentCompiling = false;
}

EffectMaterialManager::~EffectMaterialManager()
{
    delete m_compiler;
    m_compiler = nullptr;
    m_effectDeserializer = nullptr;
    DumpUnreleasedMaterial();
    assert(m_sourceMaterials.empty());
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnInit()
{
    m_onEffectMaterialCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnEffectMaterialCompiled(c); });
    Frameworks::EventPublisher::Subscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnCompileEffectMaterialFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(EffectCompiler::CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);

    m_doCompilingEffectMaterial = std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoCompilingEffectMaterial(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestCompileEffectMaterial), m_doCompilingEffectMaterial);

    EffectMaterialSource::OnDuplicatedEmpty = [this](const EffectMaterialSourcePtr& eff)
    {
        ReleaseEffectMaterialSource(eff);
    };
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnTick()
{
    if (m_isCurrentCompiling) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_requestLock };
    if (m_requests.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_compiler);
    auto request = m_requests.front();
    m_currentCompilingRuid = request->GetRuid();
    m_currentCompilingEffectName = request->GetDto().Name();
    m_requests.pop();
    auto policy = request->GetDto().ConvertToPolicy(m_effectDeserializer);
    auto proceed = m_compiler->CompileEffectMaterial(policy);
    m_isCurrentCompiling = static_cast<bool>(proceed);
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(EffectCompiler::CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;

    Frameworks::RequestBus::Unsubscribe(typeid(RequestCompileEffectMaterial), m_doCompilingEffectMaterial);
    m_doCompilingEffectMaterial = nullptr;

    return Frameworks::ServiceResult::Complete;
}

bool EffectMaterialManager::HasEffectMaterial(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(m_sourceMapLock);
    return m_sourceMaterials.find(name) != m_sourceMaterials.end();
}

EffectMaterialPtr EffectMaterialManager::QueryEffectMaterial(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(m_sourceMapLock);
    auto iter = m_sourceMaterials.find(name);
    if (iter == m_sourceMaterials.end()) return nullptr;
    return iter->second->CloneEffectMaterial();
}

/*error EffectMaterialManager::CompileEffectMaterial(const EffectMaterialPolicy& policy)
{
    std::lock_guard locker{ m_requestLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}*/

void EffectMaterialManager::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    assert(m_compiler);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectCompiler::EffectMaterialCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_currentCompilingEffectName) return;

    std::lock_guard lock(m_sourceMapLock);
    if (!ev->HasExisted())
    {
        EffectMaterialSourcePtr source = std::make_shared<EffectMaterialSource>(ev->GetEffect());
        source->LinkSource();
        m_sourceMaterials.insert_or_assign(ev->GetName(), source);
    }
    Frameworks::ResponseBus::Post(std::make_shared<CompileEffectMaterialResponse>(
        m_currentCompilingRuid, ev->GetName(), QueryEffectMaterial(ev->GetName()), ErrorCode::ok));
    m_isCurrentCompiling = false;
}

void EffectMaterialManager::OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectCompiler::CompileEffectMaterialFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("effect material %s compile failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    Frameworks::ResponseBus::Post(std::make_shared<CompileEffectMaterialResponse>(
        m_currentCompilingRuid, ev->GetName(), nullptr, ev->GetErrorCode()));
    m_isCurrentCompiling = false;
}

void EffectMaterialManager::DoCompilingEffectMaterial(const Frameworks::IRequestPtr& r)
{
    if (!r) return;
    auto req = std::dynamic_pointer_cast<RequestCompileEffectMaterial, Frameworks::IRequest>(r);
    if (!req) return;
    std::lock_guard locker{ m_requestLock };
    m_requests.push(req);
    m_needTick = true;
    //CompileEffectMaterial(cmd->GetPolicy());
}

void EffectMaterialManager::ReleaseEffectMaterialSource(const std::shared_ptr<EffectMaterialSource>& eff_source)
{
    if (!eff_source) return;
    std::lock_guard locker{ m_sourceMapLock };
    m_sourceMaterials.erase(eff_source->GetName());
}

void EffectMaterialManager::DumpUnreleasedMaterial()
{
    if (!m_sourceMaterials.empty())
    {
        for (auto& mat : m_sourceMaterials)
        {
            std::string source_name = mat.second->GetName();
            Platforms::Debug::ErrorPrintf("un-released material %s", source_name.c_str());
        }
    }
}
