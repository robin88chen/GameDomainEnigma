#include "AppConfiguration.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "FileSystem/FileSystem.h"
#include "Gateways/DtoJsonGateway.h"
#include <cassert>
#include <SceneGraph/CameraFrustumDtos.h>

using namespace LevelEditor;

std::string TOKEN_MEDIA_PATH_ID = "MediaPathId";
std::string TOKEN_DATA_PATH_ID = "DataPathId";
std::string TOKEN_PRIMARY_TARGET_NAME = "PrimaryTargetName";
std::string TOKEN_DEFAULT_RENDERER_NAME = "DefaultRendererName";
std::string TOKEN_SCENE_ROOT_NAME = "SceneRootName";
std::string TOKEN_PORTAL_MANAGEMENT_NAME = "PortalManagementName";
std::string TOKEN_CAMERA = "Camera";
std::string TOKEN_WORLD_MAP_ROOT_FOLDER = "WorldMapRootFolder";
std::string TOKEN_WORLD_MAP_PATH_ID = "WorldMapPathID";

void AppConfiguration::loadConfig()
{
    const Enigma::FileSystem::IFilePtr iFile = Enigma::FileSystem::FileSystem::Instance()->OpenFile(Enigma::FileSystem::Filename("app.cfg"), Enigma::FileSystem::Read | Enigma::FileSystem::Binary);
    const size_t file_size = iFile->Size();
    const auto read_buf = iFile->Read(0, file_size);
    Enigma::FileSystem::FileSystem::Instance()->CloseFile(iFile);
    if (read_buf)
    {
        const auto dtos = Enigma::Gateways::DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
        m_configDto = dtos[0];
    }
}

std::string AppConfiguration::mediaPathId() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_MEDIA_PATH_ID)) return v.value();
    return "";
}

std::string AppConfiguration::dataPathId() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_DATA_PATH_ID)) return v.value();
    return "";
}

std::string AppConfiguration::primaryTargetName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_PRIMARY_TARGET_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::defaultRendererName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_DEFAULT_RENDERER_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::sceneRootName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_SCENE_ROOT_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::portalManagementName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_PORTAL_MANAGEMENT_NAME)) return v.value();
    return "";
}

Enigma::Engine::GenericDto AppConfiguration::cameraDto() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<Enigma::Engine::GenericDto>(TOKEN_CAMERA)) return v.value();
    return Enigma::Engine::GenericDto{};
}

std::string AppConfiguration::worldMapRootFolderName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_WORLD_MAP_ROOT_FOLDER)) return v.value();
    return "";
}

std::string AppConfiguration::worldMapPathId() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_WORLD_MAP_PATH_ID)) return v.value();
    return "";
}

std::string AppConfiguration::cameraName() const
{
    assert(!m_configDto.IsEmpty());
    return cameraDto().getName();
}

std::string AppConfiguration::frustumName() const
{
    assert(!m_configDto.IsEmpty());
    auto camera = Enigma::SceneGraph::CameraDto::fromGenericDto(cameraDto());
    return camera.Frustum().getName();
}
