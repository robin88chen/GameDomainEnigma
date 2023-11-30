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

void DaeParserConfiguration::LoadConfig()
{
    const Enigma::FileSystem::IFilePtr iFile = Enigma::FileSystem::FileSystem::instance()->openFile("DaeParser.cfg", Enigma::FileSystem::read | Enigma::FileSystem::binary, "");
    const size_t file_size = iFile->size();
    const auto read_buf = iFile->read(0, file_size);
    Enigma::FileSystem::FileSystem::instance()->closeFile(iFile);
    if (read_buf)
    {
        const auto dtos = Enigma::Gateways::DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
        m_configDto = dtos[0];
    }
}

std::string DaeParserConfiguration::DefaultColorMeshEffectFilename()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_COLOR_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::DefaultColorMeshEffectName()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_COLOR_MESH_EFFECT_NAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::DefaultTexturedMeshEffectFilename()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_TEXTURED_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::DefaultTexturedMeshEffectName()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_TEXTURED_MESH_EFFECT_NAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::DefaultTexturedSkinMeshEffectFilename()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_TEXTURED_SKIN_MESH_EFFECT_FILENAME)) return v.value();
    return "";
}

std::string DaeParserConfiguration::DefaultTexturedSkinMeshEffectName()
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_TEXTURED_SKIN_MESH_EFFECT_NAME)) return v.value();
    return "";
}
