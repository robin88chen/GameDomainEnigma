#include "TerrainPawnAssembler.h"
#include "TerrainPawn.h"

using namespace Enigma::Terrain;

TerrainPawnAssembler::TerrainPawnAssembler(const SceneGraph::SpatialId& id) : SceneGraph::PawnAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI);
}

TerrainPawnDisassembler::TerrainPawnDisassembler() : SceneGraph::PawnDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI);
}
