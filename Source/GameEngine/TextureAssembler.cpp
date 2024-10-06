#include "TextureAssembler.h"
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
static std::string TOKEN_WIDTH_OF_CREATION = "WidthOfCreation";
static std::string TOKEN_HEIGHT_OF_CREATION = "HeightOfCreation";
static std::string TOKEN_IMAGE_FILENAMES_OF_LOAD = "ImageFilenamesOfLoad";

TextureAssembler::TextureAssembler(const TextureId& id) : m_id(id), m_factoryDesc(Texture::TYPE_RTTI.getName()), m_dimension{ 1, 1 }
{
}

GenericDto TextureAssembler::assemble() const
{
    assert(m_dimensionOfCreation.has_value() != m_imageFilenamesOfLoad.has_value());  // Either dimension or image filename should be set
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_FORMAT, m_format.fmt);
    dto.addOrUpdate(TOKEN_WIDTH, m_dimension.m_width);
    dto.addOrUpdate(TOKEN_HEIGHT, m_dimension.m_height);
    dto.addOrUpdate(TOKEN_IS_CUBE, m_isCubeTexture);
    dto.addOrUpdate(TOKEN_SURFACE_COUNT, m_surfaceCount);
    if (!m_filePaths.empty()) dto.addOrUpdate(TOKEN_FILE_PATHS, m_filePaths);
    if (m_dimensionOfCreation)
    {
        dto.addOrUpdate(TOKEN_WIDTH_OF_CREATION, m_dimensionOfCreation->m_width);
        dto.addOrUpdate(TOKEN_HEIGHT_OF_CREATION, m_dimensionOfCreation->m_height);
    }
    if ((m_imageFilenamesOfLoad) && (!m_imageFilenamesOfLoad->empty())) dto.addOrUpdate(TOKEN_IMAGE_FILENAMES_OF_LOAD, m_imageFilenamesOfLoad.value());
    return dto;
}

TextureDisassembler::TextureDisassembler() : m_factoryDesc(Texture::TYPE_RTTI.getName()), m_dimension{ 1, 1 }
{
}

void TextureDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_ID)) m_id = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_FORMAT)) m_format.fmt = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_WIDTH)) m_dimension.m_width = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_HEIGHT)) m_dimension.m_height = v.value();
    if (const auto v = dto.tryGetValue<bool>(TOKEN_IS_CUBE)) m_isCubeTexture = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_SURFACE_COUNT)) m_surfaceCount = v.value();
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_FILE_PATHS)) m_filePaths = v.value();
    if (const auto value_w = dto.tryGetValue<unsigned>(TOKEN_WIDTH_OF_CREATION))
    {
        if (const auto value_h = dto.tryGetValue<unsigned>(TOKEN_HEIGHT_OF_CREATION))
            m_dimensionOfCreation = MathLib::Dimension<unsigned>{ value_w.value(), value_h.value() };
    }
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_IMAGE_FILENAMES_OF_LOAD)) m_imageFilenamesOfLoad = v.value();
    assert(m_dimensionOfCreation.has_value() != m_imageFilenamesOfLoad.has_value());  // Either dimension or image filename should be set
}

