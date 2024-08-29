#include "RenderablePrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Primitives/PrimitiveAssembler.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"
#include "RenderableEvents.h"
#include <memory>

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

    m_primitiveRepository.lock()->registerPrimitiveFactory(MeshPrimitive::TYPE_RTTI.getName(),
        [=](const PrimitiveId& id) { return createMesh(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteMesh(id, dto); });
    m_primitiveRepository.lock()->registerPrimitiveFactory(SkinMeshPrimitive::TYPE_RTTI.getName(),
        [=](const PrimitiveId& id) { return createSkinMesh(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteSkinMesh(id, dto); });
    m_primitiveRepository.lock()->registerPrimitiveFactory(ModelPrimitive::TYPE_RTTI.getName(),
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

void RenderablePrimitiveBuilder::registerCustomMeshFactory(const std::string& rtti, const CustomMeshCreator creator, const CustomMeshConstitutor& constitutor)
{
    m_customMeshCreators.emplace(rtti, creator);
    m_customMeshConstitutors.emplace(rtti, constitutor);
    m_primitiveRepository.lock()->registerPrimitiveFactory(rtti,
        [=](const PrimitiveId& id) { return createCustomMesh(id); },
        [=](const PrimitiveId& id, const GenericDto& dto) { return constituteCustomMesh(id, dto); });
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
    auto prim = std::make_shared<MeshPrimitive>(id);
    auto disassembler = prim->disassembler();
    disassembler->disassemble(dto);
    prim->disassemble(disassembler);
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
    auto prim = std::make_shared<SkinMeshPrimitive>(id);
    auto disassembler = prim->disassembler();
    disassembler->disassemble(dto);
    prim->disassemble(disassembler);
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
    auto prim = std::make_shared<ModelPrimitive>(id);
    auto disassembler = prim->disassembler();
    disassembler->disassemble(dto);
    prim->disassemble(disassembler);
    return prim;
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::createCustomMesh(const Primitives::PrimitiveId& id)
{
    assert(m_customMeshCreators.find(id.rtti().getName()) != m_customMeshCreators.end());
    return m_customMeshCreators.at(id.rtti().getName())(id);
}

std::shared_ptr<Primitive> RenderablePrimitiveBuilder::constituteCustomMesh(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto)
{
    assert(m_customMeshConstitutors.find(id.rtti().getName()) != m_customMeshConstitutors.end());
    assert(!m_geometryRepository.expired());
    auto prim = m_customMeshConstitutors.at(id.rtti().getName())(id, dto);
    std::lock_guard locker{ m_primitivePlansLock };
    m_primitivePlans.emplace(PrimitiveHydratingPlan{ id, prim, dto });
    prim->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
    m_needTick = true;
    return prim;
}

void RenderablePrimitiveBuilder::onPrimitiveHydrated(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveHydrated, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        EventPublisher::enqueue(std::make_shared<RenderablePrimitiveHydrated>(m_currentBuildingId.value()));
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
        EventPublisher::enqueue(std::make_shared<RenderablePrimitiveHydrationFailed>(m_currentBuildingId.value(), ev->error()));
        m_currentBuildingId = std::nullopt;
    }
}
