#include "ShadowMapTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "ShadowMap/ShadowMapService.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Animators;
using namespace Enigma::SceneGraph;
using namespace Enigma::Gateways;
using namespace Enigma::InputHandlers;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::ShadowMap;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string ShadowMapTargetName = "shadowmap_target";
std::string ShadowMapRendererName = "shadowmap_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";

ShadowMapTest::ShadowMapTest(const std::string app_name) : AppDelegate(app_name)
{
}

ShadowMapTest::~ShadowMapTest()
{
}

void ShadowMapTest::InitializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::Instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../../Media/";
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(path.string(), "DataPath"));
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    if (FileSystem::Instance())
    {
        FileSystem::Instance()->AddMountPath(std::make_shared<AndroidMountPath>("", MediaPathName));

        std::string data_path = Enigma::Platforms::AndroidBridge::GetDataFilePath();
        char s[2048];
        memset(s, 0, 2048);
        memcpy(s, data_path.c_str(), data_path.size());
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(data_path, "DataPath"));
    }
#endif
}

void ShadowMapTest::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(std::make_shared<JsonFileDtoDeserializer>());
    auto input_handler_policy = std::make_shared<InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(
        CameraDtoHelper("camera").EyePosition(Vector3(-5.0f, 5.0f, -5.0f)).LookAt(Vector3(1.0f, -1.0f, 1.0f)).UpDirection(Vector3::UNIT_Y)
        .Frustum("frustum", Frustum::ProjectionType::Perspective).FrustumFov(Math::PI / 4.0f).FrustumFrontBackZ(0.1f, 100.0f)
        .FrustumNearPlaneDimension(40.0f, 30.0f).ToCameraDto());
    auto shadow_map_config = std::make_unique<ShadowMapServiceConfiguration>();
    auto shadowmap_policy = std::make_shared<ShadowMapInstallingPolicy>(ShadowMapRendererName, ShadowMapTargetName, std::move(shadow_map_config));
    auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, true);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(SceneRootName, PortalManagementName);
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, shadowmap_policy, scene_renderer_policy, game_scene_policy, game_light_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
#if TARGET_PLATFORM == PLATFORM_ANDROID
    ApplicationBridge::InitInputHandler(input_handler_policy->GetInputHandler());
#endif
    m_sceneRendererService = m_graphicMain->GetSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->GetSystemServiceAs<ShadowMapService>();
}

void ShadowMapTest::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}

void ShadowMapTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->PrepareShadowScene();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->PrepareGameScene();
}

void ShadowMapTest::RenderFrame()
{
    if (!m_shadowMapService.expired())
    {
        m_shadowMapService.lock()->RenderShadowScene();
    }
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->RenderGameScene();
        m_sceneRendererService.lock()->Flip();
    }
}

