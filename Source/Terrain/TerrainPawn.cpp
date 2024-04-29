#include "TerrainPawn.h"
#include "TerrainPawnDto.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(Terrain, TerrainPawn, Pawn);

TerrainPawn::TerrainPawn(const SpatialId& id) : Pawn(id)
{
    m_factoryDesc = FactoryDesc(TerrainPawn::TYPE_RTTI.getName()).ClaimAsInstanced(id.name() + ".pawn");
}

TerrainPawn::TerrainPawn(const SpatialId& id, const GenericDto& o) : Pawn(id, o)
{

}

TerrainPawn::~TerrainPawn()
{

}

std::shared_ptr<TerrainPawn> TerrainPawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<TerrainPawn>(id);
}

std::shared_ptr<TerrainPawn> TerrainPawn::constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& o)
{
    return std::make_shared<TerrainPawn>(id, o);
}

GenericDto TerrainPawn::serializeDto()
{
    TerrainPawnDto dto(SerializePawnDto());
    return dto.toGenericDto();
}
