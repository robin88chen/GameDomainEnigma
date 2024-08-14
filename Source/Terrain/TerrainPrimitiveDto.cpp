#include "TerrainPrimitiveDto.h"
#include "TerrainGeometry.h"
#include "TerrainPrimitive.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

TerrainPrimitiveDto::TerrainPrimitiveDto() : MeshPrimitiveDto()
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName());
    //m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.name());
}

TerrainPrimitiveDto::TerrainPrimitiveDto(const Engine::GenericDto& dto) : MeshPrimitiveDto(dto)
{
}

TerrainPrimitiveDto::TerrainPrimitiveDto(const Renderables::MeshPrimitiveDto& dto) : MeshPrimitiveDto(dto)
{
    //m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.name());
}

Enigma::Engine::GenericDto TerrainPrimitiveDto::toGenericDto() const
{
    Engine::GenericDto dto = MeshPrimitiveDto::toGenericDto();
    //dto.addRtti(Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.name()));
    return dto;
}
