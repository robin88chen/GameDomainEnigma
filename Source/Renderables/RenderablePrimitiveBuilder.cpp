#include <memory>
#include "RenderablePrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Renderer/RendererErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Primitives/PrimitiveCommands.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;

DEFINE_RTTI(Renderables, RenderablePrimitiveBuilder, ISystemService);

RenderablePrimitiveBuilder::RenderablePrimitiveBuilder(ServiceManager* mngr, const std::shared_ptr<PrimitiveRepository>& primitive_repository, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository) : ISystemService(mngr)
{
    m_primitiveRepository = primitive_repository;
    m_geometryRepository = geometry_repository;
    m_needTick = false;
    m_meshBuilder = nullptr;
}

RenderablePrimitiveBuilder::~RenderablePrimitiveBuilder()
{
}

ServiceResult RenderablePrimitiveBuilder::onInit()
{
    m_onMeshPrimitiveHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onPrimitiveHydrated(e); });
    m_onHydrateMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydratePrimitiveFailed(e); });
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), m_onMeshPrimitiveHydrated);
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed), m_onHydrateMeshPrimitiveFailed);

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

    return ServiceResult::Complete;
}

ServiceResult RenderablePrimitiveBuilder::onTick()
{
    if (m_currentBuildingId.has_value()) return ServiceResult::Pendding;
    std::lock_guard locker{ m_primitivePlansLock };
    if (m_primitivePlans.empty())
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    const auto plan = m_primitivePlans.front();
    hydrateRenderablePrimitive(plan);
    m_currentBuildingId = plan.id();
    m_primitivePlans.pop();
    return ServiceResult::Pendding;
}

ServiceResult RenderablePrimitiveBuilder::onTerm()
{
    SAFE_DELETE(m_meshBuilder);

    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), m_onMeshPrimitiveHydrated);
    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed), m_onHydrateMeshPrimitiveFailed);
    m_onMeshPrimitiveHydrated = nullptr;
    m_onHydrateMeshPrimitiveFailed = nullptr;

    return ServiceResult::Complete;
}

void RenderablePrimitiveBuilder::hydrateRenderablePrimitive(const PrimitiveHydratingPlan& plan)
{
    assert(m_meshBuilder);
    if (auto p = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(plan.primitive()))
    {
        m_meshBuilder->hydrateMeshPrimitive(p, plan.dto());
    }
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
    m_primitivePlans.emplace(PrimitiveHydratingPlan{ id, prim, dto });
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
    m_primitivePlans.emplace(PrimitiveHydratingPlan{ id, prim, dto });
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

void RenderablePrimitiveBuilder::onPrimitiveHydrated(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveHydrated, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        m_currentBuildingId = std::nullopt;
    }
}

void RenderablePrimitiveBuilder::onHydratePrimitiveFailed(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        Platforms::Debug::ErrorPrintf("mesh primitive %s build failed : %s\n",
            ev->name().c_str(), ev->error().message().c_str());
        m_currentBuildingId = std::nullopt;
    }
}
