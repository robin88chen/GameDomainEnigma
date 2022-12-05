#include "RenderablePrimitiveBuilder.h"
#include "RenderablePrimitivePolicies.h"
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "RendererErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/GeometryCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectCommands.h"
#include "GameEngine/EffectEvents.h"
#include "RenderablePrimitiveEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RenderablePrimitiveBuilder, ISystemService);

RenderablePrimitiveBuilder::RenderablePrimitiveBuilder(ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_meshBuilder = nullptr;
}

RenderablePrimitiveBuilder::~RenderablePrimitiveBuilder()
{

}

ServiceResult RenderablePrimitiveBuilder::OnInit()
{
    m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);

    m_meshBuilder = menew MeshPrimitiveBuilder();

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

    EventPublisher::Unsubscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onPrimitiveBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onBuildPrimitiveFailed = nullptr;

    return Frameworks::ServiceResult::Complete;
}

error RenderablePrimitiveBuilder::BuildPrimitive(const RenderablePrimitivePolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void RenderablePrimitiveBuilder::BuildRenderablePrimitive(const RenderablePrimitivePolicy& policy)
{
    assert(m_meshBuilder);
    m_currentPolicy = policy;
    if (typeid(policy) == typeid(MeshPrimitivePolicy))
    {
        m_meshBuilder->BuildMeshPrimitive(dynamic_cast<const MeshPrimitivePolicy&>(policy));
    }
}

void RenderablePrimitiveBuilder::OnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_currentPolicy.Name()) return;
    m_isCurrentBuilding = false;
}

void RenderablePrimitiveBuilder::OnBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_currentPolicy.Name()) return;
    Platforms::Debug::ErrorPrintf("renderable primitive %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}
