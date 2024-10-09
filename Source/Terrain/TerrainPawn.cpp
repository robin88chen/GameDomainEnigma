#include "TerrainPawn.h"
#include "TerrainPawnAssembler.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(Terrain, TerrainPawn, Pawn);

TerrainPawn::TerrainPawn(const SpatialId& id) : Pawn(id)
{
    m_factoryDesc = FactoryDesc(TerrainPawn::TYPE_RTTI.getName()).claimAsInstanced(id.name() + ".pawn");
}

TerrainPawn::~TerrainPawn()
{

}

std::shared_ptr<TerrainPawn> TerrainPawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<TerrainPawn>(id);
}

std::shared_ptr<SpatialAssembler> TerrainPawn::assembler() const
{
    return std::make_shared<TerrainPawnAssembler>(m_id);
}

std::shared_ptr<SpatialDisassembler> TerrainPawn::disassembler() const
{
    return std::make_shared<TerrainPawnDisassembler>();
}
