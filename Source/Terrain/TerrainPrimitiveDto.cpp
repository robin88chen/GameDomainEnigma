#include "TerrainPrimitiveDto.h"
#include "TerrainGeometry.h"
#include "TerrainPrimitive.h"
#include "TerrainPrimitivePolicy.h"
#include "Renderer/RenderablePrimitivePolicies.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

TerrainPrimitiveDto::TerrainPrimitiveDto() : MeshPrimitiveDto()
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName());
    m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.getName());
}

TerrainPrimitiveDto::TerrainPrimitiveDto(const Renderer::MeshPrimitiveDto& dto) : MeshPrimitiveDto(dto)
{
    //m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.getName());
}

TerrainPrimitiveDto TerrainPrimitiveDto::fromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPrimitiveDto terrain_dto(MeshPrimitiveDto::fromGenericDto(dto));
    return terrain_dto;
}

Enigma::Engine::GenericDto TerrainPrimitiveDto::toGenericDto() const
{
    Engine::GenericDto dto = MeshPrimitiveDto::toGenericDto();
    //dto.AddRtti(Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.getName()));
    return dto;
}

std::shared_ptr<GenericPolicy> TerrainPrimitiveDto::TerrainMeshConvertToPolicy(const Engine::GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    auto mesh_policy = std::dynamic_pointer_cast<Renderer::MeshPrimitivePolicy, GenericPolicy>(MeshPrimitiveDto::MeshDtoConvertToPolicy(dto, deserializer));
    return std::make_shared<TerrainPrimitivePolicy>(*mesh_policy);
}
