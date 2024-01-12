#include "TextureDto.h"
#include "GenericDto.h"
#include "Texture.h"

using namespace Enigma::Engine;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_FORMAT = "Format";
static std::string TOKEN_WIDTH = "Width";
static std::string TOKEN_HEIGHT = "Height";
static std::string TOKEN_IS_CUBE = "IsCube";
static std::string TOKEN_SURFACE_COUNT = "SurfaceCount";
static std::string TOKEN_FILE_PATHS = "FilePaths";

TextureDto::TextureDto() : m_factoryDesc(Texture::TYPE_RTTI.getName())
{
}

TextureDto TextureDto::fromGenericDto(const GenericDto& dto)
{
    TextureDto textureDto;
    textureDto.m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_ID)) textureDto.id() = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_FORMAT)) textureDto.format() = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_WIDTH)) textureDto.dimension().m_width = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_HEIGHT)) textureDto.dimension().m_height = v.value();
    if (const auto v = dto.tryGetValue<bool>(TOKEN_IS_CUBE)) textureDto.isCubeTexture() = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_SURFACE_COUNT)) textureDto.surfaceCount() = v.value();
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_FILE_PATHS)) textureDto.filePaths() = v.value();
    return textureDto;
}

GenericDto TextureDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_FORMAT, m_format.fmt);
    dto.addOrUpdate(TOKEN_WIDTH, m_dimension.m_width);
    dto.addOrUpdate(TOKEN_HEIGHT, m_dimension.m_height);
    dto.addOrUpdate(TOKEN_IS_CUBE, m_isCubeTexture);
    dto.addOrUpdate(TOKEN_SURFACE_COUNT, m_surfaceCount);
    if (!m_filePaths.empty()) dto.addOrUpdate(TOKEN_FILE_PATHS, m_filePaths);
    return dto;
}
