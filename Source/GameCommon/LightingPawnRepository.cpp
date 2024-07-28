#include "LightingPawnRepository.h"
#include "LightQuadPawn.h"
#include "LightVolumePawn.h"
#include "LightingPawnDto.h"
#include "Renderables/MeshPrimitive.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/SceneGraphCommands.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;

LightingPawnRepository::LightingPawnRepository()
{

}

LightingPawnRepository::~LightingPawnRepository()
{
    m_lightingPawns.clear();
}

void LightingPawnRepository::registerHandlers()
{

}

void LightingPawnRepository::unregisterHandlers()
{

}

void LightingPawnRepository::createAmbientLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto amb_pawn_id = SpatialId(lit->id().name() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    const auto amb_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(amb_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(amb_pawn_id);
        pawn_assembler.spatial().topLevel(true);
        pawn_assembler.primitive(amb_mesh_id).factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id(amb_pawn_id);
        lighting_pawn_dto.hostLightId(lit->id());
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(amb_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn)
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
            m_pendingLightPawnsOfAttachment.insert_or_assign(lit->id(), PendingLightPawn{ lit, std::dynamic_pointer_cast<LightingPawn>(lit_pawn) });
        }
    }
}

void LightingPawnRepository::createSunLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto sun_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    const auto sun_pawn_id = SpatialId(lit->id().name() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(sun_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(sun_pawn_id);
        pawn_assembler.spatial().topLevel(true);
        pawn_assembler.primitive(sun_mesh_id)/*.spatialFlags(m_configuration->sunLightSpatialFlags())*/.factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id(sun_pawn_id);
        lighting_pawn_dto.hostLightId(lit->id());
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(sun_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn)
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
}

void LightingPawnRepository::createPointLightVolume(const std::shared_ptr<Light>& lit)
{
    assert(lit);

    const auto vol_mesh_id = Primitives::PrimitiveId(lit->id().name() + "_lit_volume", MeshPrimitive::TYPE_RTTI);
    auto vol_pawn_id = SpatialId(lit->id().name() + "_lit_volume", LightVolumePawn::TYPE_RTTI);
    auto lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<QuerySpatial>(vol_pawn_id)->dispatch());
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(vol_pawn_id);
        pawn_assembler.spatial().topLevel(true).localTransform(lit->getLocalTransform());
        pawn_assembler.factory(FactoryDesc(LightVolumePawn::TYPE_RTTI.getName())).primitive(vol_mesh_id);
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id(vol_pawn_id);
        lighting_pawn_dto.hostLightId(lit->id());
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::dynamic_pointer_cast<LightingPawn>(std::make_shared<RequestSpatialConstitution>(vol_pawn_id, pawn_dto)->dispatch());
    }
    if (lit_pawn)
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
