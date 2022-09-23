#include "RenderBufferManager.h"
#include "RenderBufferBuilder.h"
#include "EngineErrors.h"
#include "RenderBufferEvents.h"
#include "RenderBufferCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

#include "Frameworks/CommandBus.h"

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, RenderBufferManager);

RenderBufferManager::RenderBufferManager(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    IMPLEMENT_RTTI(Enigma, Engine, RenderBufferManager, ISystemService);
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = new RenderBufferBuilder(this);
}

RenderBufferManager::~RenderBufferManager()
{
    SAFE_DELETE(m_builder);
}

Enigma::Frameworks::ServiceResult RenderBufferManager::OnInit()
{
    m_onRenderBufferBuilt =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnRenderBufferBuilt(c); });
    Frameworks::EventPublisher::Subscribe(typeid(RenderBufferBuilder::RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuildFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnRenderBufferBuildFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(RenderBufferBuildFailed), m_onRenderBufferBuildFailed);

	m_doBuildingRenderBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBuildingRenderBuffer(c); });
    Frameworks::CommandBus::Subscribe(typeid(Engine::BuildRenderBuffer), m_doBuildingRenderBuffer);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RenderBufferManager::OnTick()
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

Enigma::Frameworks::ServiceResult RenderBufferManager::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(RenderBufferBuilder::RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(RenderBufferBuildFailed), m_onRenderBufferBuildFailed);
    m_onRenderBufferBuildFailed = nullptr;

	Frameworks::CommandBus::Unsubscribe(typeid(Engine::BuildRenderBuffer), m_doBuildingRenderBuffer);
    m_doBuildingRenderBuffer = nullptr;
    return Frameworks::ServiceResult::Complete;
}

error RenderBufferManager::BuildRenderBuffer(const RenderBufferPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

bool RenderBufferManager::HasRenderBuffer(const RenderBufferSignature& signature)
{
    std::lock_guard locker{ m_bufferMapLock };
    auto it = m_renderBuffers.find(signature);
    return ((it != m_renderBuffers.end()) && (!it->second.expired()));
}

std::shared_ptr<RenderBuffer> RenderBufferManager::QueryRenderBuffer(const RenderBufferSignature& signature)
{
    std::lock_guard locker{ m_bufferMapLock };
    auto it = m_renderBuffers.find(signature);
    if (it == m_renderBuffers.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

void RenderBufferManager::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
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

void RenderBufferManager::OnRenderBufferBuildFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuildFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("render buffer %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}

void RenderBufferManager::DoBuildingRenderBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Engine::BuildRenderBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    BuildRenderBuffer(cmd->GetPolicy());
}
