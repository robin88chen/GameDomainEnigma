#include "EffectTextureMapDto.h"
#include "GameEngine/TexturePolicies.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_NAME = "TextureName";
static std::string TOKEN_FILENAME = "Filename";
static std::string TOKEN_PATH_ID = "PathId";
static std::string TOKEN_SEMANTIC = "Semantic";
static std::string TOKEN_ARRAY_INDEX = "ArrayIndex";
static std::string TOKEN_DIMENSION = "Dimension";
static std::string TOKEN_SURFACE_COUNT = "SurfaceCount";
static std::string TOKEN_JOB_TYPE = "JobType";
static std::string TOKEN_TEXTURE_MAPPINGS = "TextureMappings";

TextureMappingDto TextureMappingDto::fromGenericDto(const GenericDto& dto)
{
    TextureMappingDto tex;
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_JOB_TYPE)) tex.JobType() = static_cast<TexturePolicy::JobType>(v.value());
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_TEXTURE_NAME)) tex.TextureName() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_FILENAME)) tex.Filename() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_PATH_ID)) tex.PathId() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_SEMANTIC)) tex.Semantic() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_ARRAY_INDEX)) tex.ArrayIndex() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_DIMENSION)) tex.Dimension() = {v.value()[0], v.value()[1]};
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_SURFACE_COUNT)) tex.SurfaceCount() = v.value();
    return tex;
}

GenericDto TextureMappingDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_JOB_TYPE, static_cast<unsigned>(m_jobType));
    dto.AddOrUpdate(TOKEN_TEXTURE_NAME, m_textureName);
    dto.AddOrUpdate(TOKEN_FILENAME, m_filename);
    dto.AddOrUpdate(TOKEN_PATH_ID, m_pathId);
    dto.AddOrUpdate(TOKEN_SEMANTIC, m_semantic);
    if (m_arrayIndex)
    {
        dto.AddOrUpdate(TOKEN_ARRAY_INDEX, m_arrayIndex.value());
    }
    dto.AddOrUpdate(TOKEN_DIMENSION, std::vector<unsigned>{m_dimension.m_width, m_dimension.m_height});
    dto.AddOrUpdate(TOKEN_SURFACE_COUNT, m_scurfaceCount);
    return dto;
}

EffectTextureMapPolicy::TextureTuplePolicy TextureMappingDto::ConvertToPolicy()
{
    if (m_jobType == TexturePolicy::JobType::Load)
    {
        return std::make_tuple(m_semantic, TexturePolicy(m_textureName, m_filename, m_pathId), m_arrayIndex);
    }
    else if (m_jobType == TexturePolicy::JobType::Create)
    {
        return std::make_tuple(m_semantic, TexturePolicy(m_textureName, m_dimension, m_scurfaceCount), m_arrayIndex);
    }
    else
    {
        return std::make_tuple(m_semantic, TexturePolicy(), m_arrayIndex);
    }
}

EffectTextureMapDto EffectTextureMapDto::fromGenericDto(const GenericDto& dto)
{
    EffectTextureMapDto effect;
    if (const auto v = dto.TryGetValue<GenericDtoCollection>(TOKEN_TEXTURE_MAPPINGS))
    {
        for (auto& mapping_dto : v.value())
        {
            effect.TextureMappings().emplace_back(TextureMappingDto::fromGenericDto(mapping_dto));
        }
    }
    return effect;
}

GenericDto EffectTextureMapDto::toGenericDto()
{
    GenericDtoCollection mappings;
    for (auto& tex : m_textureMappings)
    {
        mappings.emplace_back(tex.toGenericDto());
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
