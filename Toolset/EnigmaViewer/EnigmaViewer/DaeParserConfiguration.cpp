#include "DaeParserConfiguration.h"
#include "FileSystem/FileSystem.h"
#include "Gateways/DtoJsonGateway.h"
#include "Frameworks/ExtentTypesDefine.h"

#define TOKEN_COLOR_MESH_EFFECT_FILENAME "ColorMeshEffectFilename"
#define TOKEN_COLOR_MESH_EFFECT_NAME "ColorMeshEffectName"
#define TOKEN_TEXTURED_MESH_EFFECT_FILENAME "TexturedMeshEffectFilename"
#define TOKEN_TEXTURED_MESH_EFFECT_NAME "TexturedMeshEffectName"
#define TOKEN_TEXTURED_SKIN_MESH_EFFECT_FILENAME "TexturedSkinmeshEffectFilename"
#define TOKEN_TEXTURED_SKIN_MESH_EFFECT_NAME "TexturedSkinmeshEffectName"

using namespace EnigmaViewer;

void DaeParserConfiguration::loadConfig()
{
    const Enigma::FileSystem::IFilePtr iFile = Enigma::FileSystem::FileSystem::instance()->openFile("DaeParser.cfg", Enigma::FileSystem::read | Enigma::FileSystem::binary, "");
    const size_t file_size = iFile->size();
    const auto read_buf = iFile->read(0, file_size);
    Enigma::FileSystem::FileSystem::instance()->closeFile(iFile);
    if (read_buf)
    {
        const auto dtos = std::make_shared<Enigma::Gateways::DtoJsonGateway>()->deserialize(convert_to_string(read_buf.value(), file_size));
        m_configDto = dtos[0];
    }
}

std::string DaeParserConfiguration::defaultColorMeshEffectFilename()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_COLOR_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::defaultColorMeshEffectName()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_COLOR_MESH_EFFECT_NAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::defaultTexturedMeshEffectFilename()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_TEXTURED_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::defaultTexturedMeshEffectName()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_TEXTURED_MESH_EFFECT_NAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::defaultTexturedSkinMeshEffectFilename()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_TEXTURED_SKIN_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::defaultTexturedSkinMeshEffectName()
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_TEXTURED_SKIN_MESH_EFFECT_NAME)) return v.value();
    return "";
}
