#include "TerrainPrimitiveDto.h"
#include "TerrainGeometry.h"
#include "TerrainPrimitivePolicy.h"
#include "GameEngine/GeometryDataPolicy.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

TerrainPrimitiveDto::TerrainPrimitiveDto() : MeshPrimitiveDto()
{
    m_geometryFactory = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.GetName());
}

TerrainPrimitiveDto TerrainPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPrimitiveDto terrain_dto;
    terrain_dto.MeshPrimitiveDto::FromGenericDto(dto);
    return terrain_dto;
}

Enigma::Engine::GenericDto TerrainPrimitiveDto::ToGenericDto() const
{
    Engine::GenericDto dto = MeshPrimitiveDto::ToGenericDto();
    return dto;
}

std::shared_ptr<TerrainPrimitivePolicy> TerrainPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    auto policy = std::make_shared<TerrainPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    if (m_geometry)
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometry.value());
        policy->GeometryFactoryDesc() = m_geometry.value().GetRtti();
    }
    else
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometryFactory.GetResourceFilename(), deserializer);
        policy->GeometryFactoryDesc() = m_geometryFactory;
    }
    for (auto& eff : m_effects)
    {
        policy->EffectDtos().emplace_back(EffectMaterialDto::FromGenericDto(eff));
    }
    for (auto& tex : m_textureMaps)
    {
        policy->TextureDtos().emplace_back(EffectTextureMapDto::FromGenericDto(tex));
    }
    policy->RenderListId() = m_renderListID;
    return policy;
}
