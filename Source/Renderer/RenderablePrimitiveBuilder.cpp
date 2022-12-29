#include "RenderablePrimitiveBuilder.h"
#include "RenderablePrimitivePolicies.h"
#include "MeshPrimitiveBuilder.h"
#include "ModelPrimitiveBuilder.h"
#include "RendererErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "RenderablePrimitiveEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "RenderablePrimitiveCommands.h"
#include "Frameworks/unique_ptr_dynamic_cast.hpp"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RenderablePrimitiveBuilder, ISystemService);

RenderablePrimitiveBuilder::RenderablePrimitiveBuilder(ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_meshBuilder = nullptr;
    m_modelBuilder = nullptr;
}

RenderablePrimitiveBuilder::~RenderablePrimitiveBuilder()
{
    std::lock_guard locker{ m_policiesLock };
    while (!m_policies.empty())
    {
        m_policies.pop();
    }
}

ServiceResult RenderablePrimitiveBuilder::OnInit()
{
    m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_doBuildingPrimitive = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingPrimitive(c); });
    CommandBus::Subscribe(typeid(Enigma::Renderer::BuildRenderablePrimitive), m_doBuildingPrimitive);

    m_meshBuilder = menew MeshPrimitiveBuilder();
    m_modelBuilder = menew ModelPrimitiveBuilder();

    return Frameworks::ServiceResult::Complete;
}
ServiceResult RenderablePrimitiveBuilder::OnTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    BuildRenderablePrimitive(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

ServiceResult RenderablePrimitiveBuilder::OnTerm()
{
    SAFE_DELETE(m_meshBuilder);
    SAFE_DELETE(m_modelBuilder);

    EventPublisher::Unsubscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onPrimitiveBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onBuildPrimitiveFailed = nullptr;
    CommandBus::Unsubscribe(typeid(Enigma::Renderer::BuildRenderablePrimitive), m_doBuildingPrimitive);
    m_doBuildingPrimitive = nullptr;

    return Frameworks::ServiceResult::Complete;
}

error RenderablePrimitiveBuilder::BuildPrimitive(const std::shared_ptr<RenderablePrimitivePolicy>& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void RenderablePrimitiveBuilder::BuildRenderablePrimitive(const std::shared_ptr<RenderablePrimitivePolicy>& policy)
{
    assert(m_meshBuilder);
    assert(m_modelBuilder);
    m_buildingRuid = policy->GetRuid();
    auto& p = *policy;
    if (typeid(p) == typeid(MeshPrimitivePolicy))
    {
        m_meshBuilder->BuildMeshPrimitive(std::dynamic_pointer_cast<MeshPrimitivePolicy, RenderablePrimitivePolicy>(policy));
    }
    else if (typeid(p) == typeid(ModelPrimitivePolicy))
    {
        m_modelBuilder->BuildModelPrimitive(std::dynamic_pointer_cast<ModelPrimitivePolicy, RenderablePrimitivePolicy>(policy));
    }
}

void RenderablePrimitiveBuilder::OnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_buildingRuid) return;
    m_isCurrentBuilding = false;
}

void RenderablePrimitiveBuilder::OnBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_buildingRuid) return;
    Platforms::Debug::ErrorPrintf("renderable primitive %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}

void RenderablePrimitiveBuilder::DoBuildingPrimitive(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::BuildRenderablePrimitive, ICommand>(c);
    if (!cmd) return;
    BuildPrimitive(cmd->GetPolicy());
}
