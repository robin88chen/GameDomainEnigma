#include "TerrainPrimitive.h"
#include "TerrainPrimitiveDto.h"

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

Enigma::Engine::GenericDto TerrainPrimitive::serializeDto() const
{
    TerrainPrimitiveDto dto(serializeMeshDto());
    return dto.toGenericDto();
}

