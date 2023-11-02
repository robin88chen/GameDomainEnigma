#include "TerrainPawn.h"
#include "TerrainPawnDto.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(Terrain, TerrainPawn, Pawn);

TerrainPawn::TerrainPawn(const std::string& name) : Pawn(name)
{
    m_factoryDesc = FactoryDesc(TerrainPawn::TYPE_RTTI.getName()).ClaimAsInstanced(name + ".pawn");
}

TerrainPawn::TerrainPawn(const GenericDto& o) : Pawn(o)
{

}

TerrainPawn::~TerrainPawn()
{

}

GenericDto TerrainPawn::serializeDto()
{
    TerrainPawnDto dto(SerializePawnDto());
    return dto.ToGenericDto();
}
