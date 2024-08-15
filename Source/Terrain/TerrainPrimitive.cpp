#include "TerrainPrimitive.h"
#include "TerrainPrimitiveDto.h"

using namespace Enigma::Terrain;
using namespace Enigma::Renderer;

DEFINE_RTTI(Terrain, TerrainPrimitive, MeshPrimitive);

TerrainPrimitive::TerrainPrimitive(const Primitives::PrimitiveId& id) : MeshPrimitive(id)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName()).ClaimAsInstanced(id.name() + ".terrain");
}

TerrainPrimitive::TerrainPrimitive(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository) : MeshPrimitive(id, dto, geometry_repository)
{
}

TerrainPrimitive::~TerrainPrimitive()
{

}

std::shared_ptr<Enigma::Primitives::Primitive> TerrainPrimitive::create(const Primitives::PrimitiveId& id)
{
    return std::make_shared<TerrainPrimitive>(id);
}

std::shared_ptr<Enigma::Primitives::Primitive> TerrainPrimitive::constitute(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository)
{
    return std::make_shared<TerrainPrimitive>(id, dto, geometry_repository);
}

/*Enigma::Engine::GenericDto TerrainPrimitive::serializeDto() const
{
    TerrainPrimitiveDto dto(serializeMeshDto());
    return dto.toGenericDto();
}*/

