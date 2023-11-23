#include "LevelEditorAppDelegate.h"
#include "AppConfiguration.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Platforms/MemoryMacro.h"
#include "GraphicAPIDx11/GraphicAPIDx11.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "WorldMap/WorldMapInstallingPolicy.h"
#include "WorldMap/WorldMapService.h"
#include "WorldMap/WorldMapCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"
#include "GameCommon/GameSceneEvents.h"
#include "SceneGraph/SceneFlattenTraversal.h"
#include "LevelEditorCommands.h"
#include "WorldMap/WorldMapEvents.h"
#include "WorldMap/WorldMap.h"
#include "WorldEditService.h"
#include "TerrainEditService.h"
#include "Terrain/TerrainInstallingPolicy.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "Prefabs/PrefabInstallingPolicy.h"
#include "LightEditService.h"
#include "PawnEditService.h"
#include <memory>

using namespace LevelEditor;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;
using namespace Enigma::Controllers;
using namespace Enigma::FileSystem;
using namespace Enigma::Devices;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Gateways;
using namespace Enigma::GameCommon;
using namespace Enigma::Animators;
using namespace Enigma::SceneGraph;
using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::InputHandlers;
using namespace Enigma::Terrain;
using namespace Enigma::ShadowMap;
using namespace Enigma::Prefabs;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

using namespace std::chrono_literals;

EditorAppDelegate::EditorAppDelegate()
{
    m_hasLogFile = false;
    m_graphicMain = nullptr;
    m_hwnd = nullptr;
}

EditorAppDelegate::~EditorAppDelegate()
{
}

void EditorAppDelegate::initialize(IGraphicAPI::APIVersion api_ver, IGraphicAPI::AsyncType useAsyncDevice,
    const std::string& log_filename, HWND hwnd)
{
    m_hwnd = hwnd;
    if (log_filename.length() > 0)
    {
        Logger::InitLoggerFile(log_filename);
        m_hasLogFile = true;
    }

    FileSystem::Create();
    initializeMountPaths();

    m_graphicMain = menew GraphicMain();
    m_graphicMain->InstallFrameworks();

    menew GraphicAPIDx11(useAsyncDevice);

    CoInitializeEx(NULL, COINIT_MULTITHREADED);  // for WIC Texture Loader

    installEngine();
}

void EditorAppDelegate::finalize()
{
    shutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    IGraphicAPI::Instance()->TerminateGraphicThread(); // 先跳出thread
    delete IGraphicAPI::Instance();

    m_graphicMain->ShutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::Instance();

    CoUninitialize();
}

void EditorAppDelegate::initializeMountPaths()
{
    m_appConfig = std::make_unique<AppConfiguration>();
    m_appConfig->loadConfig();

    if (FileSystem::Instance())
    {
        const auto path = std::filesystem::current_path();
        const auto mediaPath = path / "../../../Media/";
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(mediaPath.string(), m_appConfig->mediaPathId()));
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(path.string(), m_appConfig->dataPathId()));
    }
}

void EditorAppDelegate::registerMediaMountPaths(const std::string& media_path)
{
}

void EditorAppDelegate::installEngine()
{
    m_onSceneGraphChanged = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphChanged(e); });
    EventPublisher::subscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneRootCreated = std::make_shared<EventSubscriber>([=](auto e) { onSceneRootCreated(e); });
    EventPublisher::subscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    m_onWorldMapCreated = std::make_shared<EventSubscriber>([=](auto e) { onWorldMapCreated(e); });
    EventPublisher::subscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    m_onCreateWorldFailed = std::make_shared<EventSubscriber>([=](auto e) { onCreateWorldFailed(e); });
    EventPublisher::subscribe(typeid(CreateWorldMapFailed), m_onCreateWorldFailed);
    m_onWorldMapDeserialized = std::make_shared<EventSubscriber>([=](auto e) { onWorldMapDeserialized(e); });
    EventPublisher::subscribe(typeid(WorldMapDeserialized), m_onWorldMapDeserialized);
    m_onDeserializeWorldMapFailed = std::make_shared<EventSubscriber>([=](auto e) { onDeserializeWorldMapFailed(e); });
    EventPublisher::subscribe(typeid(DeserializeWorldMapFailed), m_onDeserializeWorldMapFailed);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto deferred_config = std::make_shared<DeferredRendererServiceConfiguration>();
    deferred_config->SunLightEffectName() = "DeferredShadingWithShadowSunLightPass";
    deferred_config->SunLightPassFxFileName() = "fx/DeferredShadingWithShadowSunLightPass.efx@APK_PATH";
    deferred_config->SunLightSpatialFlags() |= SpatialShadowFlags::Spatial_ShadowReceiver;
    auto deferred_renderer_policy = std::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, deferred_config);
    //auto scene_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    //auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(m_appConfig->GetDefaultRendererName(), m_appConfig->GetPrimaryTargetName(), scene_render_config);
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>());
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(m_appConfig->sceneRootName(), m_appConfig->portalManagementName());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(m_appConfig->cameraDto());
    auto world_map_policy = std::make_shared<WorldMapInstallingPolicy>();
    auto terrain_policy = std::make_shared<TerrainInstallingPolicy>();
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    auto shadow_map_config = std::make_shared<ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    auto animated_pawn_policy = std::make_shared<AnimatedPawnInstallingPolicy>();
    auto prefab_policy = std::make_shared<PrefabInstallingPolicy>(std::make_shared<JsonFileDtoDeserializer>());
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, deferred_renderer_policy, animator_policy, scene_graph_policy, input_handler_policy, game_camera_policy, world_map_policy, game_scene_policy, terrain_policy, game_light_policy, shadow_map_policy, animated_pawn_policy, prefab_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('A');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('D');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('S');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('W');
    m_sceneRenderer = m_graphicMain->getSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->getSystemServiceAs<ShadowMapService>();
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<WorldEditService>(m_graphicMain->getServiceManager(), m_graphicMain->getSystemServiceAs<WorldMapService>()));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<TerrainEditService>(m_graphicMain->getServiceManager()));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<LightEditService>(m_graphicMain->getServiceManager()));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<PawnEditService>(m_graphicMain->getServiceManager()));
}

void EditorAppDelegate::shutdownEngine()
{
    EventPublisher::unsubscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneGraphChanged = nullptr;
    EventPublisher::unsubscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    m_onSceneRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapCreated = nullptr;
    EventPublisher::unsubscribe(typeid(CreateWorldMapFailed), m_onCreateWorldFailed);
    m_onCreateWorldFailed = nullptr;
    EventPublisher::unsubscribe(typeid(WorldMapDeserialized), m_onWorldMapDeserialized);
    m_onWorldMapDeserialized = nullptr;
    EventPublisher::unsubscribe(typeid(DeserializeWorldMapFailed), m_onDeserializeWorldMapFailed);
    m_onDeserializeWorldMapFailed = nullptr;

    assert(m_graphicMain);
    m_graphicMain->getServiceManager()->shutdownSystemService(TerrainEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(WorldEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(LightEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(PawnEditService::TYPE_RTTI);
    m_graphicMain->ShutdownRenderEngine();
}

void EditorAppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void EditorAppDelegate::prepareRender()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->PrepareShadowScene();
    if (!m_sceneRenderer.expired()) m_sceneRenderer.lock()->PrepareGameScene();
}

void EditorAppDelegate::renderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->RenderShadowScene();
    if (!m_sceneRenderer.expired())
    {
        m_sceneRenderer.lock()->RenderGameScene();
        m_sceneRenderer.lock()->Flip();
    }
}

void EditorAppDelegate::onTimerElapsed()
{
    if (!m_graphicMain) return;

    frameUpdate();

    prepareRender();
    renderFrame();
    std::this_thread::sleep_for(30us);
}

void EditorAppDelegate::onSceneGraphChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraphChanged, IEvent>(e);
    if (!ev) return;
    if (m_sceneRoot.expired()) return;
    SceneFlattenTraversal traversal;
    m_sceneRoot.lock()->visitBy(&traversal);
    CommandBus::post(std::make_shared<RefreshSceneGraph>(traversal.GetSpatials()));
}

void EditorAppDelegate::onSceneRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneRootCreated, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("scene root created : " + ev->GetSceneRoot()->getSpatialName()));
    m_sceneRoot = ev->GetSceneRoot();
    SceneFlattenTraversal traversal;
    m_sceneRoot.lock()->visitBy(&traversal);
    CommandBus::post(std::make_shared<RefreshSceneGraph>(traversal.GetSpatials()));
}

void EditorAppDelegate::onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WorldMapCreated, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("world map created : " + ev->name()));
    if (!m_sceneRoot.expired())
    {
        m_sceneRoot.lock()->AttachChild(ev->world()->getRoot(), Enigma::MathLib::Matrix4::IDENTITY);
    }
}

void EditorAppDelegate::onCreateWorldFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CreateWorldMapFailed, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("create world map " + ev->name() + " failed : " + ev->error().message()));
}

void EditorAppDelegate::onWorldMapDeserialized(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WorldMapDeserialized, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("world map deserialized : " + ev->name()));
    if (!m_sceneRoot.expired())
    {
        m_sceneRoot.lock()->AttachChild(ev->world()->getRoot(), Enigma::MathLib::Matrix4::IDENTITY);
    }
}

void EditorAppDelegate::onDeserializeWorldMapFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<DeserializeWorldMapFailed, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("deserialize world map " + ev->name() + " failed : " + ev->error().message()));
}

