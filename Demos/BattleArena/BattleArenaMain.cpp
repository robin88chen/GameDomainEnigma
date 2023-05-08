#include "BattleArenaMain.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Gateways/DtoJsonGateway.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/ApplicationBridge.h"
#endif
#include "AppConfiguration.h"
#include <string>

using namespace BattleArena;

BattleArenaMain::BattleArenaMain(const std::string& app_name) : AppDelegate(app_name)
{

}

void BattleArenaMain::InitializeMountPaths()
{
    m_appConfig = std::make_unique<AppConfiguration>();
    m_appConfig->LoadConfig();

#if TARGET_PLATFORM == PLATFORM_WIN32
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        const auto path = std::filesystem::current_path();
        const auto mediaPath = path / "../../../Media/";
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::StdMountPath>(mediaPath.string(), m_appConfig->GetMediaPathId()));
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::StdMountPath>(path.string(), m_appConfig->GetDataPathId()));
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::AndroidMountPath>("", m_appConfig->GetMediaPathId()));

        std::string data_path = Enigma::Platforms::AndroidBridge::GetDataFilePath();
        char s[2048];
        memset(s, 0, 2048);
        memcpy(s, data_path.c_str(), data_path.size());
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::StdMountPath>(data_path, m_appConfig->GetDataPathId()));
    }
#endif
}

void BattleArenaMain::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_shared<Enigma::Engine::DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<Enigma::Engine::EngineInstallingPolicy>(std::make_shared<Enigma::Gateways::JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<Enigma::Renderer::RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<Enigma::Animators::AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<Enigma::SceneGraph::SceneGraphInstallingPolicy>(std::make_shared<Enigma::Gateways::JsonFileDtoDeserializer>());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<Enigma::GameCommon::GameCameraInstallingPolicy>(Enigma::SceneGraph::CameraDto::FromGenericDto(m_appConfig->GetCameraDto()));
    auto scene_renderer_policy = std::make_shared<Enigma::GameCommon::SceneRendererInstallingPolicy>(m_appConfig->GetDefaultRendererName(), m_appConfig->GetPrimaryTargetName(), true);
    auto game_scene_policy = std::make_shared<Enigma::GameCommon::GameSceneInstallingPolicy>(m_appConfig->GetSceneRootName(), m_appConfig->GetPortalManagementName());
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, scene_renderer_policy, game_scene_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRendererService = m_graphicMain->GetSystemServiceAs<Enigma::GameCommon::SceneRendererService>();
}

void BattleArenaMain::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}

void BattleArenaMain::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->PrepareGameScene();
}

void BattleArenaMain::RenderFrame()
{
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->RenderGameScene();
        m_sceneRendererService.lock()->Flip();
    }
}

