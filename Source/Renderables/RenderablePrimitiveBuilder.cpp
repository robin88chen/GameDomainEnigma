#include "RenderablePrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Primitives/PrimitiveAssembler.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Primitives/PrimitiveEvents.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "MeshPrimitive.h"
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
    m_onPrimitiveConstituted = eventSubscription(typeid(PrimitiveConstituted), [=](auto e) { this->onPrimitiveConstituted(e); });
    m_onPrimitiveConstitutionFailed = eventSubscription(typeid(PrimitiveConstitutionFailed), [=](auto e) { this->onPrimitiveConstitutionFailed(e); });
    m_onMeshPrimitiveHydrated = eventSubscription(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), [=](auto e) { this->onMeshPrimitiveHydrated(e); });
    m_onMeshPrimitiveHydrationFailed = eventSubscription(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrationFailed), [=](auto e) { this->onMeshPrimitiveHydrationFailed(e); });

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

    releaseSubscription(typeid(PrimitiveConstituted), m_onPrimitiveConstituted);
    m_onPrimitiveConstituted = nullptr;
    releaseSubscription(typeid(PrimitiveConstitutionFailed), m_onPrimitiveConstitutionFailed);
    m_onPrimitiveConstitutionFailed = nullptr;
    releaseSubscription(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), m_onMeshPrimitiveHydrated);
    m_onMeshPrimitiveHydrated = nullptr;
    releaseSubscription(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrationFailed), m_onMeshPrimitiveHydrationFailed);
    m_onMeshPrimitiveHydrationFailed = nullptr;

    return ServiceResult::Complete;
}

void RenderablePrimitiveBuilder::hydrateRenderablePrimitive(const PrimitiveHydratingPlan& plan)
{
    assert(m_meshBuilder);
    if (auto p = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(plan.primitive()))
    {
        m_meshBuilder->hydrateMeshPrimitive(p);
    }
}

void RenderablePrimitiveBuilder::onPrimitiveConstituted(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimitiveConstituted, IEvent>(e);
    if (!ev) return;
    if (ev->primitive() == nullptr) return;
    if (ev->primitive()->typeInfo().isDerived(MeshPrimitive::TYPE_RTTI))
    {
        std::lock_guard locker{ m_primitivePlansLock };
        m_primitivePlans.emplace(PrimitiveHydratingPlan{ ev->id(), ev->primitive() });
        ev->primitive()->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
        m_needTick = true;
    }
}

void RenderablePrimitiveBuilder::onPrimitiveConstitutionFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimitiveConstitutionFailed, IEvent>(e);
    if (!ev) return;
}

void RenderablePrimitiveBuilder::onMeshPrimitiveHydrated(const IEventPtr& e)
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

void RenderablePrimitiveBuilder::onMeshPrimitiveHydrationFailed(const IEventPtr& e)
{
    if (!e) return;
    if (!m_currentBuildingId) return;
    if (const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveHydrationFailed, IEvent>(e))
    {
        if (ev->id() != m_currentBuildingId.value()) return;
        Platforms::Debug::ErrorPrintf("mesh primitive %s build failed : %s\n",
            ev->name().c_str(), ev->error().message().c_str());
        EventPublisher::enqueue(std::make_shared<RenderablePrimitiveHydrationFailed>(m_currentBuildingId.value(), ev->error()));
        m_currentBuildingId = std::nullopt;
    }
}
