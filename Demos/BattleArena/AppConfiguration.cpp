
#include "Platforms/PlatformConfig.h"
#include "AppConfiguration.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "FileSystem/FileSystem.h"
#include "Gateways/DtoJsonGateway.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Platforms/AndroidBridge.h"
#endif
#include <cassert>

using namespace BattleArena;

std::string TOKEN_MEDIA_PATH_ID = "MediaPathId";
std::string TOKEN_DATA_PATH_ID = "DataPathId";
std::string TOKEN_PRIMARY_TARGET_NAME = "PrimaryTargetName";
std::string TOKEN_DEFAULT_RENDERER_NAME = "DefaultRendererName";
std::string TOKEN_SCENE_ROOT_NAME = "SceneRootName";
std::string TOKEN_PORTAL_MANAGEMENT_NAME = "PortalManagementName";
std::string TOKEN_CAMERA = "Camera";

void AppConfiguration::LoadConfig()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    std::string data_path = std::filesystem::current_path().string();
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    std::string data_path = Enigma::Platforms::AndroidBridge::GetDataFilePath();
#endif
    const Enigma::FileSystem::IFilePtr iFile = Enigma::FileSystem::FileSystem::Instance()->OpenFile(data_path + "/app.cfg", "rb", "");
    const size_t file_size = iFile->Size();
    const auto read_buf = iFile->Read(0, file_size);
    Enigma::FileSystem::FileSystem::Instance()->CloseFile(iFile);
    if (read_buf)
    {
        const auto dtos = Enigma::Gateways::DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
        m_configDto = dtos[0];
    }
}

std::string AppConfiguration::GetMediaPathId() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_MEDIA_PATH_ID)) return v.value();
    return "";
}

std::string AppConfiguration::GetDataPathId() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_DATA_PATH_ID)) return v.value();
    return "";
}

std::string AppConfiguration::GetPrimaryTargetName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_PRIMARY_TARGET_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::GetDefaultRendererName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_DEFAULT_RENDERER_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::GetSceneRootName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_SCENE_ROOT_NAME)) return v.value();
    return "";
}

std::string AppConfiguration::GetPortalManagementName() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<std::string>(TOKEN_PORTAL_MANAGEMENT_NAME)) return v.value();
    return "";
}

Enigma::Engine::GenericDto AppConfiguration::GetCameraDto() const
{
    assert(!m_configDto.IsEmpty());
    if (auto v = m_configDto.TryGetValue<Enigma::Engine::GenericDto>(TOKEN_CAMERA)) return v.value();
    return Enigma::Engine::GenericDto();
}
