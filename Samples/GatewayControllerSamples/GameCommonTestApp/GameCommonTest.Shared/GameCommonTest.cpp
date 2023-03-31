#include "GameCommonTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/PortalZoneNode.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "CameraDtoMaker.h"
#include "Frameworks/menew_make_shared.hpp"
#include "GameCommon/SceneRendererService.h"
#include "GameCommon/GameSceneService.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraphMaker.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::SceneGraph;
using namespace Enigma::Controllers;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::Platforms;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";

GameCommonTest::GameCommonTest(const std::string app_name) : AppDelegate(app_name)
{
}

GameCommonTest::~GameCommonTest()
{
}

void GameCommonTest::InitializeMountPaths()
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

void GameCommonTest::InstallEngine()
{
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<Enigma::Animators::AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>(), std::make_shared<JsonFileEffectProfileDeserializer>());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(CameraDtoMaker::MakeCameraDto());
    auto scene_renderer_policy = stdext::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, true);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(SceneRootName, PortalManagementName);
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, scene_renderer_policy, game_scene_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRendererService = m_graphicMain->GetSystemServiceAs<SceneRendererService>();

    PrimitiveMeshMaker::MakeSavedFloorGeometry("floor");
    PrimitiveMeshMaker::MakeSavedDoorGeometry("door");
    PrimitiveMeshMaker::MakeSavedBoardGeometry("board");
    auto outside_dto = SceneGraphMaker::MakeOutsideRegion("outside_region", Matrix4::IDENTITY);
    outside_dto.TheFactoryDesc().ClaimAsDeferred(outside_dto.Name() + ".node", "DataPath");
    outside_dto.ChildNames() = {};
    auto dtos = { outside_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>("outside_region", dtos));
}

void GameCommonTest::ShutdownEngine()
{
    m_sceneRoot = nullptr;

    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void GameCommonTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->PrepareGameScene();
}

void GameCommonTest::RenderFrame()
{
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->RenderGameScene();
        m_sceneRendererService.lock()->Flip();
    }
}

void GameCommonTest::OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    auto outside_node = std::dynamic_pointer_cast<PortalZoneNode, Spatial>(top_spatials[0]);
    if (!outside_node) return;
    auto scene_service = m_graphicMain->GetSystemServiceAs<GameSceneService>();
    if (!scene_service) return;
    error er = scene_service->AttachOutsideZone(outside_node);
}
