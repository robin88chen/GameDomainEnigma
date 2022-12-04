#include "RenderablePrimitiveBuilder.h"
#include "RenderablePrimitivePolicies.h"
#include "MeshPrimitive.h"
#include "RendererErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/GeometryCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "RenderablePrimitiveEvents.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RenderablePrimitiveBuilder, ISystemService);

RenderablePrimitiveBuilder::RenderablePrimitiveBuilder(ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
}

RenderablePrimitiveBuilder::~RenderablePrimitiveBuilder()
{

}

ServiceResult RenderablePrimitiveBuilder::OnInit()
{
    m_onGeometryDataBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnGeometryDataBuilt(e); });
    EventPublisher::Subscribe(typeid(GeometryDataBuilt), m_onGeometryDataBuilt);
    m_onBuildGeometryDataFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildGeometryDataFailed(e); });
    EventPublisher::Subscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryDataFailed);
    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildRenderBufferFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

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
    EventPublisher::Unsubscribe(typeid(GeometryDataBuilt), m_onGeometryDataBuilt);
    m_onGeometryDataBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryDataFailed);
    m_onBuildGeometryDataFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

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
    m_builtPrimitive = nullptr;
    m_builtGeometry = nullptr;
    m_builtRenderBuffer = nullptr;
    m_currentPolicy = policy;
    if (typeid(policy) == typeid(MeshPrimitivePolicy))
    {
        BuildMeshPrimitive(dynamic_cast<const MeshPrimitivePolicy&>(policy));
    }
}

void RenderablePrimitiveBuilder::BuildMeshPrimitive(const MeshPrimitivePolicy& policy)
{
    m_builtPrimitive = std::make_shared<MeshPrimitive>(policy.Name());
    Frameworks::CommandBus::Post(std::make_shared<BuildGeometryData>(policy.GeometryPolicy()));
}

void RenderablePrimitiveBuilder::OnGeometryDataBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GeometryDataBuilt, IEvent>(e);
    if (!ev) return;
    if (typeid(m_currentPolicy) == typeid(MeshPrimitivePolicy))
    {
        const MeshPrimitivePolicy& mesh_policy = dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy);
        if (ev->GetName() != mesh_policy.GeometryPolicy().Name()) return;
    }
    else return;
    m_builtGeometry = ev->GetGeometryData();
    RenderBufferPolicy buffer;
    buffer.m_signature = m_builtGeometry->MakeRenderBufferSignature();
    buffer.m_renderBufferName = buffer.m_signature.GetName();
    buffer.m_vtxBufferName = buffer.m_renderBufferName + ".vtx";
    buffer.m_idxBufferName = buffer.m_renderBufferName + ".idx";
    buffer.m_sizeofVertex = m_builtGeometry->SizeofVertex();
    buffer.m_vtxBufferSize = m_builtGeometry->GetVertexCapacity() * m_builtGeometry->SizeofVertex();
    buffer.m_idxBufferSize = m_builtGeometry->GetIndexCapacity();
    buffer.m_vtxBuffer = m_builtGeometry->GetVertexMemory();
    if (m_builtGeometry->GetIndexCapacity() > 0)
    {
        buffer.m_idxBuffer = m_builtGeometry->GetIndexMemory();
    }
    CommandBus::Post(std::make_shared<BuildRenderBuffer>(buffer));
}

void RenderablePrimitiveBuilder::OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildGeometryDataFailed, IEvent>(e);
    if (!ev) return;
    if (typeid(m_currentPolicy) == typeid(MeshPrimitivePolicy))
    {
        const MeshPrimitivePolicy& mesh_policy = dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy);
        if (ev->GetName() != mesh_policy.GeometryPolicy().Name()) return;
    }
    else return;
    m_isCurrentBuilding = false;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy).Name(), ev->GetErrorCode()));
}

void RenderablePrimitiveBuilder::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_builtGeometry);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (typeid(m_currentPolicy) == typeid(MeshPrimitivePolicy))
    {
        const MeshPrimitivePolicy& mesh_policy = dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy);
        if (ev->GetName() != mesh_policy.GeometryPolicy().Name()) return;
    }
    else return;
    m_builtRenderBuffer = ev->GetBuffer();
    std::dynamic_pointer_cast<MeshPrimitive, Primitive>(m_builtPrimitive)->LinkGeometryData(m_builtGeometry, m_builtRenderBuffer);
}

void RenderablePrimitiveBuilder::OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (typeid(m_currentPolicy) == typeid(MeshPrimitivePolicy))
    {
        const MeshPrimitivePolicy& mesh_policy = dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy);
        if (ev->GetName() != mesh_policy.GeometryPolicy().Name()) return;
    }
    else return;
    m_isCurrentBuilding = false;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(dynamic_cast<const MeshPrimitivePolicy&>(m_currentPolicy).Name(), ev->GetErrorCode()));

}
