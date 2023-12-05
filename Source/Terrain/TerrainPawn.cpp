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

GenericDto TerrainPawn::serializeDto()
{
    TerrainPawnDto dto(SerializePawnDto());
    return dto.toGenericDto();
}
