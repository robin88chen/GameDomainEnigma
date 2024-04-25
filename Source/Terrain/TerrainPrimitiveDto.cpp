#include "TerrainPrimitiveDto.h"
#include "TerrainGeometry.h"
#include "TerrainPrimitive.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

TerrainPrimitiveDto::TerrainPrimitiveDto() : MeshPrimitiveDto()
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName());
    //m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.getName());
}

TerrainPrimitiveDto::TerrainPrimitiveDto(const Renderables::MeshPrimitiveDto& dto) : MeshPrimitiveDto(dto)
{
    //m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.getName());
}

TerrainPrimitiveDto TerrainPrimitiveDto::fromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPrimitiveDto terrain_dto{ MeshPrimitiveDto{dto} };
    return terrain_dto;
}

Enigma::Engine::GenericDto TerrainPrimitiveDto::toGenericDto() const
{
    Engine::GenericDto dto = MeshPrimitiveDto::toGenericDto();
    //dto.addRtti(Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName()));
    return dto;
}
