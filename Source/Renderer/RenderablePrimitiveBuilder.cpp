#include "RenderablePrimitiveBuilder.h"
#include "RenderablePrimitivePolicies.h"
#include "MeshPrimitiveBuilder.h"
#include "ModelPrimitiveBuilder.h"
#include "RendererErrors.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ResponseBus.h"
#include "RenderablePrimitiveRequests.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "RenderablePrimitiveResponses.h"

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
    m_onMeshPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    m_onModelPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    m_onBuildMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    m_onBuildModelPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(ModelPrimitiveBuilder::ModelPrimitiveBuilt), m_onModelPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::Subscribe(typeid(ModelPrimitiveBuilder::BuildModelPrimitiveFailed), m_onBuildModelPrimitiveFailed);

    m_doBuildingPrimitive = std::make_shared<RequestSubscriber>([=](auto r) { this->DoBuildingPrimitive(r); });
    RequestBus::Subscribe(typeid(RequestBuildRenderablePrimitive), m_doBuildingPrimitive);

    m_meshBuilder = menew MeshPrimitiveBuilder();
    m_modelBuilder = menew ModelPrimitiveBuilder();

    return ServiceResult::Complete;
}
ServiceResult RenderablePrimitiveBuilder::OnTick()
{
    if (m_isCurrentBuilding) return ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    BuildRenderablePrimitive(std::get<Ruid>(m_policies.front()), std::get<std::shared_ptr<RenderablePrimitivePolicy>>(m_policies.front()));
    m_policies.pop();
    m_isCurrentBuilding = true;
    return ServiceResult::Pendding;
}

ServiceResult RenderablePrimitiveBuilder::OnTerm()
{
    SAFE_DELETE(m_meshBuilder);
    SAFE_DELETE(m_modelBuilder);

    EventPublisher::Unsubscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Unsubscribe(typeid(ModelPrimitiveBuilder::ModelPrimitiveBuilt), m_onModelPrimitiveBuilt);
    EventPublisher::Unsubscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::Unsubscribe(typeid(ModelPrimitiveBuilder::BuildModelPrimitiveFailed), m_onBuildModelPrimitiveFailed);
    m_onMeshPrimitiveBuilt = nullptr;
    m_onModelPrimitiveBuilt = nullptr;
    m_onBuildMeshPrimitiveFailed = nullptr;
    m_onBuildModelPrimitiveFailed = nullptr;

    RequestBus::Unsubscribe(typeid(RequestBuildRenderablePrimitive), m_doBuildingPrimitive);
    m_doBuildingPrimitive = nullptr;

    return ServiceResult::Complete;
}

error RenderablePrimitiveBuilder::BuildPrimitive(const Ruid& requester_ruid, const std::shared_ptr<RenderablePrimitivePolicy>& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push({ requester_ruid, policy });
    m_needTick = true;
    return ErrorCode::ok;
}

void RenderablePrimitiveBuilder::BuildRenderablePrimitive(const Ruid& requester_ruid, const std::shared_ptr<RenderablePrimitivePolicy>& policy)
{
    assert(m_meshBuilder);
    assert(m_modelBuilder);
    m_buildingRuid = requester_ruid;
    auto& p = *policy;
    if (typeid(p) == typeid(MeshPrimitivePolicy))
    {
        m_meshBuilder->BuildMeshPrimitive(m_buildingRuid, std::dynamic_pointer_cast<MeshPrimitivePolicy, RenderablePrimitivePolicy>(policy));
    }
    else if (typeid(p) == typeid(ModelPrimitivePolicy))
    {
        m_modelBuilder->BuildModelPrimitive(m_buildingRuid, std::dynamic_pointer_cast<ModelPrimitivePolicy, RenderablePrimitivePolicy>(policy));
    }
}

void RenderablePrimitiveBuilder::OnPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveBuilt, IEvent>(e))
    {
        if (ev->GetRuid() != m_buildingRuid) return;
        ResponseBus::Post(std::make_shared<BuildRenderablePrimitiveResponse>(m_buildingRuid, ev->GetName(), ev->GetPrimitive(), ErrorCode::ok));
        m_isCurrentBuilding = false;
    }
    else if (const auto ev = std::dynamic_pointer_cast<ModelPrimitiveBuilder::ModelPrimitiveBuilt, IEvent>(e))
    {
        if (ev->GetRuid() != m_buildingRuid) return;
        ResponseBus::Post(std::make_shared<BuildRenderablePrimitiveResponse>(m_buildingRuid, ev->GetName(), ev->GetPrimitive(), ErrorCode::ok));
        m_isCurrentBuilding = false;
    }
}

void RenderablePrimitiveBuilder::OnBuildPrimitiveFailed(const IEventPtr& e)
{
    if (!e) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::BuildMeshPrimitiveFailed, IEvent>(e))
    {
        if (ev->GetRuid() != m_buildingRuid) return;
        Platforms::Debug::ErrorPrintf("mesh primitive %s build failed : %s\n",
            ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
        ResponseBus::Post(std::make_shared<BuildRenderablePrimitiveResponse>(m_buildingRuid, ev->GetName(), nullptr, ev->GetErrorCode()));
        m_isCurrentBuilding = false;
    }
    else if (const auto ev = std::dynamic_pointer_cast<ModelPrimitiveBuilder::BuildModelPrimitiveFailed, IEvent>(e))
    {
        if (ev->GetRuid() != m_buildingRuid) return;
        Platforms::Debug::ErrorPrintf("model primitive %s build failed : %s\n",
                       ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
        ResponseBus::Post(std::make_shared<BuildRenderablePrimitiveResponse>(m_buildingRuid, ev->GetName(), nullptr, ev->GetErrorCode()));
        m_isCurrentBuilding = false;
    }
}

void RenderablePrimitiveBuilder::DoBuildingPrimitive(const IRequestPtr& r)
{
    if (!r) return;
    const auto req = std::dynamic_pointer_cast<RequestBuildRenderablePrimitive, IRequest>(r);
    if (!req) return;
    BuildPrimitive(req->GetRuid(), req->GetPolicy());
}
