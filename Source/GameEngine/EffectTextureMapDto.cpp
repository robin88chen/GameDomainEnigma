#include "EffectTextureMapDto.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_NAME = "TextureName";
static std::string TOKEN_FILENAME = "Filename";
static std::string TOKEN_PATH_ID = "PathId";
static std::string TOKEN_SEMANTIC = "Semantic";
static std::string TOKEN_ARRAY_INDEX = "ArrayIndex";
static std::string TOKEN_TEXTURE_MAPPINGS = "TextureMappings";

TextureMappingDto TextureMappingDto::FromGenericDto(const GenericDto& dto)
{
    TextureMappingDto tex;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_TEXTURE_NAME)) tex.TextureName() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_FILENAME)) tex.Filename() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_PATH_ID)) tex.PathId() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_SEMANTIC)) tex.Semantic() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_ARRAY_INDEX)) tex.ArrayIndex() = v.value();
    return tex;
}

GenericDto TextureMappingDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_TEXTURE_NAME, m_textureName);
    dto.AddOrUpdate(TOKEN_FILENAME, m_filename);
    dto.AddOrUpdate(TOKEN_PATH_ID, m_pathId);
    dto.AddOrUpdate(TOKEN_SEMANTIC, m_semantic);
    if (m_arrayIndex)
    {
        dto.AddOrUpdate(TOKEN_ARRAY_INDEX, m_arrayIndex.value());
    }
    return dto;
}

EffectTextureMapPolicy::TextureTuplePolicy TextureMappingDto::ConvertToPolicy()
{
    return std::make_tuple(m_semantic, TexturePolicy(m_textureName, m_filename, m_pathId), m_arrayIndex);
}

EffectTextureMapDto EffectTextureMapDto::FromGenericDto(const GenericDto& dto)
{
    EffectTextureMapDto effect;
    if (const auto v = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_TEXTURE_MAPPINGS))
    {
        for (auto& mapping_dto : v.value())
        {
            effect.TextureMappings().emplace_back(TextureMappingDto::FromGenericDto(mapping_dto));
        }
    }
    return effect;
}

GenericDto EffectTextureMapDto::ToGenericDto()
{
    std::vector<GenericDto> mappings;
    for (auto& tex : m_textureMappings)
    {
        mappings.emplace_back(tex.ToGenericDto());
    }
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_TEXTURE_MAPPINGS, mappings);
    return dto;
}

EffectTextureMapPolicy EffectTextureMapDto::ConvertToPolicy()
{
    EffectTextureMapPolicy policy;
    for (auto& mapping : m_textureMappings)
    {
        policy.GetTuplePolicies().emplace_back(mapping.ConvertToPolicy());
    }
    return policy;
}
