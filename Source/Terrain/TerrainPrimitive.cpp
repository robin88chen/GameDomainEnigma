#include "TerrainPrimitive.h"
#include "TerrainPrimitiveAssembler.h"

using namespace Enigma::Terrain;
using namespace Enigma::Renderer;

DEFINE_RTTI(Terrain, TerrainPrimitive, MeshPrimitive);

TerrainPrimitive::TerrainPrimitive(const Primitives::PrimitiveId& id) : MeshPrimitive(id)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName()).ClaimAsInstanced(id.name() + ".terrain");
}

TerrainPrimitive::~TerrainPrimitive()
{

}

std::shared_ptr<Enigma::Primitives::PrimitiveAssembler> TerrainPrimitive::assembler() const
{
    return std::make_shared<TerrainPrimitiveAssembler>(m_id.origin());
}

std::shared_ptr<Enigma::Primitives::PrimitiveDisassembler> TerrainPrimitive::disassembler() const
{
    return std::make_shared<TerrainPrimitiveDisassembler>();
}

