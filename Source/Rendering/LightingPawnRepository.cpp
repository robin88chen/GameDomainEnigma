#include "LightingPawnRepository.h"
#include "LightQuadPawn.h"
#include "LightVolumePawn.h"
#include "LightingPawnAssemblers.h"
#include "Renderables/MeshPrimitive.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::Rendering;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;

LightingPawnRepository::LightingPawnRepository()
{

}

LightingPawnRepository::~LightingPawnRepository()
{
    m_lightingPawns.clear();
}

void LightingPawnRepository::registerHandlers()
{
    m_onLightAttached = std::make_shared<EventSubscriber>([=](auto e) { onLightAttached(e); });
    EventPublisher::subscribe(typeid(NodeChildAttached), m_onLightAttached);
    m_onLightAttachmentFailed = std::make_shared<EventSubscriber>([=](auto e) { onLightAttachmentFailed(e); });
    EventPublisher::subscribe(typeid(NodeChildAttachmentFailed), m_onLightAttachmentFailed);
}

void LightingPawnRepository::unregisterHandlers()
{
    EventPublisher::unsubscribe(typeid(NodeChildAttached), m_onLightAttached);
    m_onLightAttached = nullptr;
    EventPublisher::unsubscribe(typeid(NodeChildAttachmentFailed), m_onLightAttachmentFailed);
    m_onLightAttachmentFailed = nullptr;
}

void LightingPawnRepository::createAmbientLightPawn(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto amb_pawn_id = SpatialId(lit->id().name() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    const auto amb_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(amb_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        std::shared_ptr<LightQuadPawnAssembler> assembler = std::make_shared<LightQuadPawnAssembler>(amb_pawn_id);
        assembler->topLevel(true);
        assembler->primitiveId(amb_mesh_id);
        assembler->hostLightId(lit->id());
        auto pawn_dto = assembler->assemble();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(amb_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn) tryCompleteLightPawnAttachment(lit, lit_pawn);
}

void LightingPawnRepository::createSunLightPawn(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto sun_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    const auto sun_pawn_id = SpatialId(lit->id().name() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(sun_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        std::shared_ptr<LightQuadPawnAssembler> assembler = std::make_shared<LightQuadPawnAssembler>(sun_pawn_id);
        assembler->topLevel(true);
        assembler->primitiveId(sun_mesh_id);
        assembler->hostLightId(lit->id());
        auto pawn_dto = assembler->assemble();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(sun_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn) tryCompleteLightPawnAttachment(lit, lit_pawn);
}

void LightingPawnRepository::createPointLightPawn(const std::shared_ptr<Light>& lit, const SpatialId& present_camera_id)
{
    assert(lit);

    const auto vol_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_volume", MeshPrimitive::TYPE_RTTI);
    auto vol_pawn_id = SpatialId(lit->id().name() + "_lit_volume", LightVolumePawn::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(vol_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        std::shared_ptr<LightVolumePawnAssembler> assembler = std::make_shared<LightVolumePawnAssembler>(vol_pawn_id);
        assembler->topLevel(true);
        assembler->localTransform(lit->getLocalTransform());
        assembler->primitiveId(vol_mesh_id);
        assembler->hostLightId(lit->id());
        assembler->presentCameraId(present_camera_id);
        auto pawn_dto = assembler->assemble();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(vol_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn) tryCompleteLightPawnAttachment(lit, lit_pawn);
}

void LightingPawnRepository::removeLightingPawn(const SpatialId& lit_id)
{
    if (const auto it = m_lightingPawns.find(lit_id); it != m_lightingPawns.end())
    {
        if (!it->second.expired())
        {
            std::shared_ptr<LightingPawn> lit_pawn = it->second.lock();
            lit_pawn->unregisterHandlers();
            std::make_shared<DeleteSceneSpatial>(lit_pawn->id())->enqueue();
        }
        m_lightingPawns.erase(it);
    }
}

std::shared_ptr<LightingPawn> LightingPawnRepository::findLightingPawn(const SpatialId& lit_id)
{
    if (const auto it = m_lightingPawns.find(lit_id); it != m_lightingPawns.end())
    {
        return it->second.lock();
    }
    return nullptr;
}

void LightingPawnRepository::onLightAttached(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::NodeChildAttached>(e);
    if ((!ev) || (!ev->child())) return;
    if (!ev->child()->typeInfo().isDerived(Light::TYPE_RTTI)) return;
    resolvePendingLightPawnAttachment(std::dynamic_pointer_cast<Light>(ev->child()));
}

void LightingPawnRepository::onLightAttachmentFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::NodeChildAttachmentFailed>(e);
    if (!ev) return;
    if (!ev->childId().rtti().isDerived(Light::TYPE_RTTI)) return;
    if (const auto it = m_pendingLightPawnsOfAttachment.find(ev->childId()); it != m_pendingLightPawnsOfAttachment.end())
    {
        m_pendingLightPawnsOfAttachment.erase(it);
    }
}

void LightingPawnRepository::tryCompleteLightPawnAttachment(const std::shared_ptr<SceneGraph::Light>& lit, const std::shared_ptr<LightingPawn>& lit_pawn)
{
    lit_pawn->registerHandlers();
    if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
    {
        parent->attachChild(lit_pawn, lit->getLocalTransform());
        m_lightingPawns.insert_or_assign(lit->id(), lit_pawn);
        lit_pawn->notifySpatialRenderStateChanged();
    }
    else
    {
        m_pendingLightPawnsOfAttachment.insert_or_assign(lit->id(), PendingLightPawn{ lit, lit_pawn });
    }
}

void LightingPawnRepository::resolvePendingLightPawnAttachment(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    if (const auto it = m_pendingLightPawnsOfAttachment.find(lit->id()); it != m_pendingLightPawnsOfAttachment.end())
    {
        assert((it->second.light) && (it->second.light->id() == lit->id()) && (it->second.pawn));
        if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
        {
            parent->attachChild(it->second.pawn, lit->getLocalTransform());
            m_lightingPawns.insert_or_assign(lit->id(), it->second.pawn);
            it->second.pawn->notifySpatialRenderStateChanged();
            m_pendingLightPawnsOfAttachment.erase(it);
        }
    }
}
