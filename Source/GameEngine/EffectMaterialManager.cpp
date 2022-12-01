#include "EffectMaterialManager.h"
#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include "EffectCompiler.h"
#include "EffectEvents.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, EffectMaterialManager, ISystemService);

EffectMaterialManager::EffectMaterialManager(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_compiler = menew EffectCompiler(this);
    m_isCurrentCompiling = false;
}

EffectMaterialManager::~EffectMaterialManager()
{
    delete m_compiler;
    m_compiler = nullptr;
    DumpUnreleasedMaterial();
    assert(m_sourceMaterials.empty());
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnInit()
{
    m_onEffectMaterialCompiled =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnEffectMaterialCompiled(c); });
    Frameworks::EventPublisher::Subscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnCompileEffectMaterialFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);

    EffectMaterialSource::OnDuplicatedEmpty = [this](const EffectMaterialSourcePtr& eff)
    {
        ReleaseEffectMaterialSource(eff);
    };
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnTick()
{
    if (m_isCurrentCompiling) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_compiler);
    m_compiler->CompileEffectMaterial(m_policies.front());
    m_policies.pop();
    m_isCurrentCompiling = true;
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult EffectMaterialManager::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;

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

error EffectMaterialManager::CompileEffectMaterial(const EffectMaterialPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void EffectMaterialManager::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    assert(m_compiler);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectCompiler::EffectMaterialCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    std::lock_guard<std::recursive_mutex> lock(m_sourceMapLock);

    EffectMaterialSourcePtr source = std::make_shared<EffectMaterialSource>(ev->GetEffect());
    source->LinkSource();
    m_sourceMaterials.try_emplace(ev->GetFilename(), source);
    Frameworks::EventPublisher::Post(std::make_shared<EffectMaterialCompiled>(ev->GetFilename(), QueryEffectMaterial(ev->GetFilename())));
    m_isCurrentCompiling = false;
}

void EffectMaterialManager::OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("effect material %s compile failed : %s\n",
        ev->GetFilename().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentCompiling = false;
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
