#include "RenderablePrimitiveDtos.h"
#include "RenderablePrimitivePolicies.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/GeometryDataDto.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/EffectMaterialPolicy.h"
#include "GameEngine/EffectTextureMapPolicy.h"
#include "MeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_GEOMETRY_NAME = "GeometryName";
static std::string TOKEN_RAW_GEOMETRY = "RawGeometry";
static std::string TOKEN_GEOMETRY_FACTORY = "GeometryFactory";
static std::string TOKEN_EFFECTS = "Effects";
static std::string TOKEN_TEXTURE_MAPS = "TextureMaps";

MeshPrimitiveDto::MeshPrimitiveDto() : m_geometryFactory(GeometryData::TYPE_RTTI.GetName())
{
}

MeshPrimitiveDto MeshPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    MeshPrimitiveDto mesh;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) mesh.Name() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_GEOMETRY_NAME)) mesh.GeometryName() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_RAW_GEOMETRY)) mesh.TheGeometry() = GeometryDataDto::FromGenericDto(v.value());
    if (const auto v = dto.TryGetValue<FactoryDesc>(TOKEN_GEOMETRY_FACTORY)) mesh.GeometryFactoryDesc() = v.value();
    if (const auto ary = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_EFFECTS))
    {
        for (auto& eff_dto : ary.value())
        {
            mesh.Effects().emplace_back(EffectMaterialDto::FromGenericDto(eff_dto));
        }
    }
    if (const auto ary = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_TEXTURE_MAPS))
    {
        for (auto& tex_dto : ary.value())
        {
            mesh.TextureMaps().emplace_back(EffectTextureMapDto::FromGenericDto(tex_dto));
        }
    }
    return mesh;
}

GenericDto MeshPrimitiveDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(MeshPrimitive::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_GEOMETRY_NAME, m_geometryName);
    if (m_geometry)
    {
        dto.AddOrUpdate(TOKEN_RAW_GEOMETRY, m_geometry.value().ToGenericDto());
    }
    dto.AddOrUpdate(TOKEN_GEOMETRY_FACTORY, m_geometryFactory);
    std::vector<GenericDto> eff_dtos;
    for (auto& eff : m_effects)
    {
        eff_dtos.emplace_back(eff.ToGenericDto());
    }
    dto.AddOrUpdate(TOKEN_EFFECTS, eff_dtos);
    std::vector<GenericDto> tex_dtos;
    for (auto& tex : m_textureMaps)
    {
        tex_dtos.emplace_back(tex.ToGenericDto());
    }
    dto.AddOrUpdate(TOKEN_TEXTURE_MAPS, tex_dtos);
    return dto;
}

std::shared_ptr<MeshPrimitivePolicy> MeshPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer,
    const std::shared_ptr<IEffectCompilingProfileDeserializer>& effect_deserializer)
{
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    policy->TheEffectDeserializer() = effect_deserializer;
    if (m_geometry)
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometry.value().ToGenericDto());
    }
    else
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometryFactory.GetResourceFilename(), deserializer);
    }
    for (auto& eff : m_effects)
    {
        policy->EffectPolicies().emplace_back(eff.ConvertToPolicy(effect_deserializer));
    }
    for (auto& tex : m_textureMaps)
    {
        policy->TexturePolicies().emplace_back(tex.ConvertToPolicy());
    }
    return policy;
}
