#include "SkinMeshPrimitiveAssembler.h"
#include "SkinMeshPrimitive.h"

using namespace Enigma::Renderables;

SkinMeshPrimitiveAssembler::SkinMeshPrimitiveAssembler(const Primitives::PrimitiveId& id)
    : MeshPrimitiveAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(SkinMeshPrimitive::TYPE_RTTI);
}

SkinMeshPrimitiveDisassembler::SkinMeshPrimitiveDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(SkinMeshPrimitive::TYPE_RTTI);
}
