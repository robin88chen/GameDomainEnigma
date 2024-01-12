#include <memory>
#include "RenderablePrimitiveBuilder.h"
#include "RenderablePrimitivePolicies.h"
#include "MeshPrimitiveBuilder.h"
#include "ModelPrimitiveBuilder.h"
#include "RendererErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "RenderableCommands.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "RenderableEvents.h"
#include "GameEngine/PrimitiveCommands.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RenderablePrimitiveBuilder, ISystemService);

RenderablePrimitiveBuilder::RenderablePrimitiveBuilder(ServiceManager* mngr, const std::shared_ptr<PrimitiveRepository>& primitive_repository, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository) : ISystemService(mngr) //, m_buildingRuid()
{
    m_primitiveRepository = primitive_repository;
    m_geometryRepository = geometry_repository;
    //m_dtoDeserializer = dto_deserializer;
    m_needTick = false;
    //m_isCurrentBuilding = false;
    m_meshBuilder = nullptr;
    //m_modelBuilder = nullptr;
}

RenderablePrimitiveBuilder::~RenderablePrimitiveBuilder()
{
    /*std::lock_guard locker{ m_policiesLock };
    while (!m_policies.empty())
    {
        m_policies.pop();
    }*/
}

ServiceResult RenderablePrimitiveBuilder::onInit()
{
    m_onMeshPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onPrimitiveBuilt(e); });
    //m_onModelPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onPrimitiveBuilt(e); });
    m_onBuildMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildPrimitiveFailed(e); });
    //m_onBuildModelPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildPrimitiveFailed(e); });
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    //EventPublisher::subscribe(typeid(ModelPrimitiveBuilder::ModelPrimitiveBuilt), m_onModelPrimitiveBuilt);
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    //EventPublisher::subscribe(typeid(ModelPrimitiveBuilder::BuildModelPrimitiveFailed), m_onBuildModelPrimitiveFailed);

    m_buildPrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->buildPrimitive(c); });
    CommandBus::subscribe(typeid(BuildRenderablePrimitive), m_buildPrimitive);

    m_primitiveRepository.lock()->factory()->registerPrimitiveFactory(MeshPrimitive::TYPE_RTTI.getName(),
        [=](const PrimitiveId& id) { return createMesh(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteMesh(id, dto); });
    m_primitiveRepository.lock()->factory()->registerPrimitiveFactory(SkinMeshPrimitive::TYPE_RTTI.getName(),
        [=](const PrimitiveId& id) { return createSkinMesh(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteSkinMesh(id, dto); });
    m_primitiveRepository.lock()->factory()->registerPrimitiveFactory(ModelPrimitive::TYPE_RTTI.getName(),
        [=](const PrimitiveId& id) { return createModel(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteModel(id, dto); });

    m_meshBuilder = menew MeshPrimitiveBuilder();
    //m_modelBuilder = menew ModelPrimitiveBuilder();

    return ServiceResult::Complete;
}

ServiceResult RenderablePrimitiveBuilder::onTick()
{
    /*if (m_isCurrentBuilding) return ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    buildRenderablePrimitive(std::get<Ruid>(m_policies.front()), std::get<std::shared_ptr<RenderablePrimitivePolicy>>(m_policies.front()));
    m_policies.pop();
    m_isCurrentBuilding = true;*/
    if (m_currentBuildingId.has_value()) return ServiceResult::Pendding;
    std::lock_guard locker{ m_primitivePlansLock };
    if (m_primitivePlans.empty())
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    const auto plan = m_primitivePlans.front();
    buildRenderablePrimitive(plan);
    m_currentBuildingId = plan.id();
    m_primitivePlans.pop();
    return ServiceResult::Pendding;
}

ServiceResult RenderablePrimitiveBuilder::onTerm()
{
    SAFE_DELETE(m_meshBuilder);
    //SAFE_DELETE(m_modelBuilder);

    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    //EventPublisher::unsubscribe(typeid(ModelPrimitiveBuilder::ModelPrimitiveBuilt), m_onModelPrimitiveBuilt);
    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    //EventPublisher::unsubscribe(typeid(ModelPrimitiveBuilder::BuildModelPrimitiveFailed), m_onBuildModelPrimitiveFailed);
    m_onMeshPrimitiveBuilt = nullptr;
    //m_onModelPrimitiveBuilt = nullptr;
    m_onBuildMeshPrimitiveFailed = nullptr;
    //m_onBuildModelPrimitiveFailed = nullptr;

    CommandBus::unsubscribe(typeid(BuildRenderablePrimitive), m_buildPrimitive);
    m_buildPrimitive = nullptr;

    return ServiceResult::Complete;
}

/*error RenderablePrimitiveBuilder::buildPrimitive(const Ruid& requester_ruid, const GenericDto& dto)
{
    auto policy = std::dynamic_pointer_cast<RenderablePrimitivePolicy>(dto.convertToPolicy(m_dtoDeserializer));
    if (!policy) return ErrorCode::invalidPrimitiveDto;
    std::lock_guard locker{ m_policiesLock };
    m_policies.push({ requester_ruid, policy });
    m_needTick = true;
    return ErrorCode::ok;
}*/

/*void RenderablePrimitiveBuilder::buildRenderablePrimitive(const Ruid& requester_ruid, const std::shared_ptr<RenderablePrimitivePolicy>& policy)
{
    assert(m_meshBuilder);
    assert(m_modelBuilder);
    m_buildingRuid = requester_ruid;
    if (auto p = std::dynamic_pointer_cast<MeshPrimitivePolicy, RenderablePrimitivePolicy>(policy))
    {
        //m_meshBuilder->BuildMeshPrimitive(m_buildingRuid, p);
    }
    else if (auto p = std::dynamic_pointer_cast<ModelPrimitivePolicy, RenderablePrimitivePolicy>(policy))
    {
        m_modelBuilder->BuildModelPrimitive(m_buildingRuid, p);
    }
}*/

void RenderablePrimitiveBuilder::buildRenderablePrimitive(const PrimitiveBuildingPlan& plan)
{
    assert(m_meshBuilder);
    //assert(m_modelBuilder);
    if (auto p = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(plan.primitive()))
    {
        m_meshBuilder->constituteLazyMeshPrimitive(p, plan.dto());
    }
    //else if (auto p = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(primitive))
    //{
      //  m_modelBuilder->BuildModelPrimitive(m_buildingRuid, id, p, dto);
    //}
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::createMesh(const PrimitiveId& id)
{
    return std::make_shared<MeshPrimitive>(id);
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::constituteMesh(const PrimitiveId& id, const GenericDto& dto)
{
    assert(!m_geometryRepository.expired());
    auto prim = std::make_shared<MeshPrimitive>(id, dto, m_geometryRepository.lock());
    std::lock_guard locker{ m_primitivePlansLock };
    m_primitivePlans.emplace(PrimitiveBuildingPlan{ id, prim, dto });
    prim->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
    m_needTick = true;
    return prim;
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::createSkinMesh(const PrimitiveId& id)
{
    return std::make_shared<SkinMeshPrimitive>(id);
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::constituteSkinMesh(const PrimitiveId& id, const GenericDto& dto)
{
    assert(!m_geometryRepository.expired());
    auto prim = std::make_shared<SkinMeshPrimitive>(id, dto, m_geometryRepository.lock());
    std::lock_guard locker{ m_primitivePlansLock };
    m_primitivePlans.emplace(PrimitiveBuildingPlan{ id, prim, dto });
    prim->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
    m_needTick = true;
    return prim;
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::createModel(const PrimitiveId& id)
{
    return std::make_shared<ModelPrimitive>(id);
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::constituteModel(const PrimitiveId& id, const GenericDto& dto)
{
    return std::make_shared<ModelPrimitive>(id, dto);
}

void RenderablePrimitiveBuilder::onPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveBuilt, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        EventPublisher::post(std::make_shared<RenderablePrimitiveBuilt>(ev->id(), ev->primitive()));
        m_currentBuildingId = std::nullopt;
        //m_isCurrentBuilding = false;
    }
    /*else if (const auto ev = std::dynamic_pointer_cast<ModelPrimitiveBuilder::ModelPrimitiveBuilt, IEvent>(e))
    {
        if (ev->getRuid() != m_buildingRuid) return;
        EventPublisher::post(std::make_shared<RenderablePrimitiveBuilt>(m_buildingRuid, ev->getName(), ev->GetPrimitive()));
        m_isCurrentBuilding = false;
    }*/
}

void RenderablePrimitiveBuilder::onBuildPrimitiveFailed(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::BuildMeshPrimitiveFailed, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        Platforms::Debug::ErrorPrintf("mesh primitive %s build failed : %s\n",
            ev->name().c_str(), ev->error().message().c_str());
        EventPublisher::post(std::make_shared<BuildRenderablePrimitiveFailed>(ev->id(), ev->error()));
        m_currentBuildingId = std::nullopt;
        //m_isCurrentBuilding = false;
    }
    /*else if (const auto ev = std::dynamic_pointer_cast<ModelPrimitiveBuilder::BuildModelPrimitiveFailed, IEvent>(e))
    {
        if (ev->getRuid() != m_buildingRuid) return;
        Platforms::Debug::ErrorPrintf("model primitive %s build failed : %s\n",
            ev->getName().c_str(), ev->GetErrorCode().message().c_str());
        EventPublisher::post(std::make_shared<BuildRenderablePrimitiveFailed>(m_buildingRuid, ev->getName(), ev->GetErrorCode()));
        m_isCurrentBuilding = false;
    }*/
}

void RenderablePrimitiveBuilder::buildPrimitive(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<BuildRenderablePrimitive>(c);
    if (!cmd) return;
    /*error er = buildPrimitive(cmd->getRuid(), cmd->primitiveDto());
    if (er)
    {
        EventPublisher::post(std::make_shared<BuildRenderablePrimitiveFailed>(m_buildingRuid, cmd->primitiveDto().getName(), er));
    }*/
}
