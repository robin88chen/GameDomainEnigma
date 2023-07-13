#include "TerrainPrimitiveDto.h"
#include "TerrainGeometry.h"
#include "TerrainPrimitive.h"
#include "TerrainPrimitivePolicy.h"
#include "Renderer/RenderablePrimitivePolicies.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

TerrainPrimitiveDto::TerrainPrimitiveDto() : MeshPrimitiveDto()
{
    m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.GetName());
}

TerrainPrimitiveDto::TerrainPrimitiveDto(const Renderer::MeshPrimitiveDto& dto) : MeshPrimitiveDto(dto)
{
    m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.GetName());
}

TerrainPrimitiveDto TerrainPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPrimitiveDto terrain_dto(MeshPrimitiveDto::FromGenericDto(dto));
    return terrain_dto;
}

Enigma::Engine::GenericDto TerrainPrimitiveDto::ToGenericDto() const
{
    Engine::GenericDto dto = MeshPrimitiveDto::ToGenericDto();
    dto.AddRtti(Engine::FactoryDesc(TerrainPrimitive::TYPE_RTTI.GetName()));
    dto.SetPolicyConverter([=, *this](auto d) { return ConvertToPolicy(d); });
    return dto;
}

std::shared_ptr<GenericPolicy> TerrainPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) const
{
    auto mesh_policy = std::dynamic_pointer_cast<Renderer::MeshPrimitivePolicy, GenericPolicy>(MeshPrimitiveDto::ConvertToPolicy(deserializer));
    return std::make_shared<TerrainPrimitivePolicy>(*mesh_policy);
}
