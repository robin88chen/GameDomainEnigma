#include "TerrainPrimitiveAssembler.h"
#include "TerrainPrimitive.h"

using namespace Enigma::Terrain;

TerrainPrimitiveAssembler::TerrainPrimitiveAssembler(const Primitives::PrimitiveId& id) : Renderables::MeshPrimitiveAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI);
}

TerrainPrimitiveDisassembler::TerrainPrimitiveDisassembler() : Renderables::MeshPrimitiveDisassembler()
{
}

