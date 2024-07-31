#include "AppConfiguration.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "FileSystem/FileSystem.h"
#include "Gateways/DtoJsonGateway.h"
#include "SceneGraph/CameraDtos.h"
#include <cassert>

using namespace LevelEditor;

std::string TOKEN_MEDIA_PATH_ID = "MediaPathId";
std::string TOKEN_DATA_PATH_ID = "DataPathId";
std::string TOKEN_WORLD_DATA_RELATIVE_PATH = "WorldDataRelativePath";
std::string TOKEN_WORLD_DATA_PATH_ID = "WorldDataPathId";
std::string TOKEN_TERRAIN_RELATIVE_PATH = "TerrainRelativePath";
std::string TOKEN_PRIMARY_TARGET_NAME = "PrimaryTargetName";
std::string TOKEN_DEFAULT_RENDERER_NAME = "DefaultRendererName";
std::string TOKEN_SCENE_ROOT_ID = "SceneRootId";
std::string TOKEN_PORTAL_MANAGEMENT_ID = "PortalManagementId";
std::string TOKEN_CAMERA = "Camera";
std::string TOKEN_WORLD_MAP_ROOT_FOLDER = "WorldMapRootFolder";
std::string TOKEN_WORLD_MAP_PATH_ID = "WorldMapPathID";

void AppConfiguration::loadConfig()
{
    const Enigma::FileSystem::IFilePtr iFile = Enigma::FileSystem::FileSystem::instance()->openFile(Enigma::FileSystem::Filename("app.cfg"), Enigma::FileSystem::read | Enigma::FileSystem::binary);
    const size_t file_size = iFile->size();
    const auto read_buf = iFile->read(0, file_size);
    Enigma::FileSystem::FileSystem::instance()->closeFile(iFile);
    if (read_buf)
    {
        const auto dtos = std::make_shared<Enigma::Gateways::DtoJsonGateway>()->deserialize(convert_to_string(read_buf.value(), file_size));
        m_configDto = dtos[0];
    }
}

std::string AppConfiguration::mediaPathId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_MEDIA_PATH_ID)) return v.value();
    return "";
}

/*std::string AppConfiguration::dataPathId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_DATA_PATH_ID)) return v.value();
    return "";
}*/

std::string AppConfiguration::worldDataRelativePath() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_WORLD_DATA_RELATIVE_PATH)) return v.value();
    return "";
}

std::string AppConfiguration::terrainRelativePath() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_TERRAIN_RELATIVE_PATH)) return v.value();
    return "";
}
/*std::string AppConfiguration::worldDataPathId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_WORLD_DATA_PATH_ID)) return v.value();
    return "";
}*/

std::string AppConfiguration::primaryTargetName() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_PRIMARY_TARGET_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::defaultRendererName() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_DEFAULT_RENDERER_NAME)) return v.value();
    return "";
}

Enigma::SceneGraph::SpatialId AppConfiguration::sceneRootId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::vector<std::string>>(TOKEN_SCENE_ROOT_ID)) return v.value();
    return {};
}

Enigma::SceneGraph::SpatialId AppConfiguration::portalManagementId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::vector<std::string>>(TOKEN_PORTAL_MANAGEMENT_ID)) return v.value();
    return {};
}

Enigma::Engine::GenericDto AppConfiguration::cameraDto() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<Enigma::Engine::GenericDto>(TOKEN_CAMERA)) return v.value();
    return Enigma::Engine::GenericDto{};
}

std::string AppConfiguration::worldMapRootFolderName() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_WORLD_MAP_ROOT_FOLDER)) return v.value();
    return "";
}

std::string AppConfiguration::worldMapPathId() const
{
    assert(!m_configDto.isEmpty());
    if (auto v = m_configDto.tryGetValue<std::string>(TOKEN_WORLD_MAP_PATH_ID)) return v.value();
    return "";
}

Enigma::SceneGraph::SpatialId AppConfiguration::cameraId() const
{
    assert(!m_configDto.isEmpty());
    Enigma::SceneGraph::CameraDto camera = Enigma::SceneGraph::CameraDto::fromGenericDto(cameraDto());
    return camera.id();
}

std::string AppConfiguration::frustumName() const
{
    assert(!m_configDto.isEmpty());
    auto camera = Enigma::SceneGraph::CameraDto::fromGenericDto(cameraDto());
    return camera.Frustum().getName();
}
