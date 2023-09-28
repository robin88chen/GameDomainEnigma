#include "EnigmaGameMain.h"
#include "AppConfiguration.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Gateways/DtoJsonGateway.h"

using namespace EnigmaGame;

EnigmaGameMain::EnigmaGameMain(const std::string& app_name) : AppDelegate(app_name)
{

}

void EnigmaGameMain::InitializeMountPaths()
{
    m_appConfig = std::make_unique<AppConfiguration>();
    m_appConfig->LoadConfig();

    if (Enigma::FileSystem::FileSystem::Instance())
    {
        const auto path = std::filesystem::current_path();
        const auto mediaPath = path / "../../../Media/";
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::StdMountPath>(mediaPath.string(), m_appConfig->GetMediaPathId()));
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::StdMountPath>(path.string(), m_appConfig->GetDataPathId()));
    }
}

void EnigmaGameMain::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_shared<Enigma::Engine::DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<Enigma::Engine::EngineInstallingPolicy>(std::make_shared<Enigma::Gateways::JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<Enigma::Renderer::RenderSystemInstallingPolicy>();
    auto deferred_config = std::make_shared<Enigma::GameCommon::DeferredRendererServiceConfiguration>();
    deferred_config->SunLightEffectName() = "DeferredShadingWithShadowSunLightPass";
    deferred_config->SunLightPassFxFileName() = "fx/DeferredShadingWithShadowSunLightPass.efx@APK_PATH";
    deferred_config->SunLightSpatialFlags() |= Enigma::ShadowMap::SpatialShadowFlags::Spatial_ShadowReceiver;
    auto deferred_renderer_policy = std::make_shared<Enigma::GameCommon::DeferredRendererInstallingPolicy>(m_appConfig->GetDefaultRendererName(), m_appConfig->GetPrimaryTargetName(), deferred_config);
    auto animator_policy = std::make_shared<Enigma::Animators::AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<Enigma::SceneGraph::SceneGraphInstallingPolicy>(std::make_shared<Enigma::Gateways::JsonFileDtoDeserializer>());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<Enigma::GameCommon::GameCameraInstallingPolicy>(m_appConfig->GetCameraDto());
    //auto scene_renderer_policy = std::make_shared<Enigma::GameCommon::SceneRendererInstallingPolicy>(m_appConfig->GetDefaultRendererName(), m_appConfig->GetPrimaryTargetName(), true);
    auto game_scene_policy = std::make_shared<Enigma::GameCommon::GameSceneInstallingPolicy>(m_appConfig->GetSceneRootName(), m_appConfig->GetPortalManagementName());
    auto game_light_policy = std::make_shared<Enigma::GameCommon::GameLightInstallingPolicy>();
    auto shadow_map_config = std::make_shared<Enigma::ShadowMap::ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<Enigma::ShadowMap::ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, game_scene_policy, deferred_renderer_policy, game_light_policy, shadow_map_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRendererService = m_graphicMain->GetSystemServiceAs<Enigma::GameCommon::SceneRendererService>();
    m_shadowMapService = m_graphicMain->GetSystemServiceAs<Enigma::ShadowMap::ShadowMapService>();
}

void EnigmaGameMain::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}

void EnigmaGameMain::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->PrepareShadowScene();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->PrepareGameScene();
}

void EnigmaGameMain::RenderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->RenderShadowScene();
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->RenderGameScene();
        m_sceneRendererService.lock()->Flip();
    }
}
