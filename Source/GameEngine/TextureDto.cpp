#include "TextureDto.h"
#include "GenericDto.h"

using namespace Enigma::Engine;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_FORMAT = "Format";
static std::string TOKEN_WIDTH = "Width";
static std::string TOKEN_HEIGHT = "Height";
static std::string TOKEN_IS_CUBE = "IsCube";
static std::string TOKEN_IS_MULTI = "IsMulti";
static std::string TOKEN_FILE_PATHS = "FilePaths";

TextureDto TextureDto::fromGenericDto(const GenericDto& dto)
{
    TextureDto textureDto;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_ID)) textureDto.id() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_FORMAT)) textureDto.format() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_WIDTH)) textureDto.dimension().m_width = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_HEIGHT)) textureDto.dimension().m_height = v.value();
    if (const auto v = dto.TryGetValue<bool>(TOKEN_IS_CUBE)) textureDto.isCubeTexture() = v.value();
    if (const auto v = dto.TryGetValue<bool>(TOKEN_IS_MULTI)) textureDto.isMultiTexture() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_FILE_PATHS)) textureDto.filePaths() = v.value();
    return textureDto;
}

GenericDto TextureDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_ID, m_id.name());
    dto.AddOrUpdate(TOKEN_FORMAT, m_format.fmt);
    dto.AddOrUpdate(TOKEN_WIDTH, m_dimension.m_width);
    dto.AddOrUpdate(TOKEN_HEIGHT, m_dimension.m_height);
    dto.AddOrUpdate(TOKEN_IS_CUBE, m_isCubeTexture);
    dto.AddOrUpdate(TOKEN_IS_MULTI, m_isMultiTexture);
    dto.AddOrUpdate(TOKEN_FILE_PATHS, m_filePaths);
    return dto;
}
