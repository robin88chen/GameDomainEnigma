#include "RenderBufferRepository.h"
#include "RenderBufferBuilder.h"
#include "EngineErrors.h"
#include "RenderBufferEvents.h"
#include "RenderBufferCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, RenderBufferRepository);

RenderBufferRepository::RenderBufferRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    IMPLEMENT_RTTI(Enigma, Engine, RenderBufferRepository, ISystemService);
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = new RenderBufferBuilder(this);
}

RenderBufferRepository::~RenderBufferRepository()
{
    SAFE_DELETE(m_builder);
}

Enigma::Frameworks::ServiceResult RenderBufferRepository::OnInit()
{
    m_onRenderBufferBuilt =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnRenderBufferBuilt(c); });
    Frameworks::EventPublisher::Subscribe(typeid(RenderBufferBuilder::RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnBuildRenderBufferFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_doBuildingRenderBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBuildingRenderBuffer(c); });
    Frameworks::CommandBus::Subscribe(typeid(Engine::BuildRenderBuffer), m_doBuildingRenderBuffer);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RenderBufferRepository::OnTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_builder);
    m_builder->BuildRenderBuffer(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult RenderBufferRepository::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(RenderBufferBuilder::RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Engine::BuildRenderBuffer), m_doBuildingRenderBuffer);
    m_doBuildingRenderBuffer = nullptr;
    return Frameworks::ServiceResult::Complete;
}

error RenderBufferRepository::BuildRenderBuffer(const RenderBufferPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

bool RenderBufferRepository::HasRenderBuffer(const RenderBufferSignature& signature)
{
    std::lock_guard locker{ m_bufferMapLock };
    auto it = m_renderBuffers.find(signature);
    return ((it != m_renderBuffers.end()) && (!it->second.expired()));
}

std::shared_ptr<RenderBuffer> RenderBufferRepository::QueryRenderBuffer(const RenderBufferSignature& signature)
{
    std::lock_guard locker{ m_bufferMapLock };
    auto it = m_renderBuffers.find(signature);
    if (it == m_renderBuffers.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

void RenderBufferRepository::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_builder);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuilder::RenderBufferBuilt, Frameworks::IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_bufferMapLock };
    m_renderBuffers.try_emplace(ev->GetSignature(), ev->GetBuffer());
    m_isCurrentBuilding = false;
    Frameworks::EventPublisher::Post(std::make_shared<RenderBufferBuilt>(ev->GetName(), ev->GetSignature(), ev->GetBuffer()));
}

void RenderBufferRepository::OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("render buffer %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}

void RenderBufferRepository::DoBuildingRenderBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Engine::BuildRenderBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    BuildRenderBuffer(cmd->GetPolicy());
}
