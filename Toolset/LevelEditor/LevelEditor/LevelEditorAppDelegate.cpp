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
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "WorldMap/WorldMapInstallingPolicy.h"
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
#include "LightEditService.h"
#include "PawnEditService.h"
#include "Gateways/DtoJsonGateway.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "Controllers/ControllerEvents.h"
#include "GameCommon/GameSceneCommands.h"
#include "SceneGraph/PortalAssemblers.h"
#include "GameCommon/GameLightCommands.h"
#include "GeometryDataFileStoreMapper.h"
#include "SceneGraphFileStoreMapper.h"
#include "TextureFileStoreMapper.h"
#include "WorldMapFileStoreMapper.h"
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
using namespace Enigma::FileStorage;

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

    FileSystem::create();
    initializeMountPaths();

    m_graphicMain = menew GraphicMain();
    m_graphicMain->installFrameworks();

    menew GraphicAPIDx11(useAsyncDevice);

    //CoInitializeEx(NULL, COINIT_MULTITHREADED);  // for WIC Texture Loader

    installEngine();
}

void EditorAppDelegate::finalize()
{
    shutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    IGraphicAPI::instance()->TerminateGraphicThread(); // 先跳出thread
    delete IGraphicAPI::instance();

    m_graphicMain->shutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::instance();

    //CoUninitialize();
}

void EditorAppDelegate::initializeMountPaths()
{
    m_appConfig = std::make_unique<AppConfiguration>();
    m_appConfig->loadConfig();

    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        //auto dataPath = path / "Data";
        auto mediaPath = path / "../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), m_appConfig->mediaPathId()));
        /*if (!std::filesystem::exists(dataPath))
        {
            std::filesystem::create_directory(dataPath);
        }
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(dataPath.string(), m_appConfig->dataPathId()));*/
        auto worldDataPath = mediaPath / m_appConfig->worldDataRelativePath();
        if (!std::filesystem::exists(worldDataPath))
        {
            std::filesystem::create_directory(worldDataPath);
        }
        //FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(worldDataPath.string(), m_appConfig->worldDataPathId()));
        auto terrainPath = mediaPath / m_appConfig->terrainRelativePath();
        if (!std::filesystem::exists(terrainPath))
        {
            std::filesystem::create_directory(terrainPath);
        }
    }
}

void EditorAppDelegate::registerMediaMountPaths(const std::string& media_path)
{
}

void EditorAppDelegate::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([=](auto e) { onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onSceneRootCreated = std::make_shared<EventSubscriber>([=](auto e) { onSceneRootCreated(e); });
    EventPublisher::subscribe(typeid(PortalSceneRootCreated), m_onSceneRootCreated);
    m_onSceneGraphChanged = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphChanged(e); });
    EventPublisher::subscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    /*m_onWorldMapCreated = std::make_shared<EventSubscriber>([=](auto e) { onWorldMapCreated(e); });
    EventPublisher::subscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    m_onCreateWorldFailed = std::make_shared<EventSubscriber>([=](auto e) { onCreateWorldFailed(e); });
    EventPublisher::subscribe(typeid(CreateWorldMapFailed), m_onCreateWorldFailed);
    m_onWorldMapDeserialized = std::make_shared<EventSubscriber>([=](auto e) { onWorldMapDeserialized(e); });
    EventPublisher::subscribe(typeid(WorldMapDeserialized), m_onWorldMapDeserialized);
    m_onDeserializeWorldMapFailed = std::make_shared<EventSubscriber>([=](auto e) { onDeserializeWorldMapFailed(e); });
    EventPublisher::subscribe(typeid(DeserializeWorldMapFailed), m_onDeserializeWorldMapFailed);
    */
    assert(m_graphicMain);

    auto mediaPathId = m_appConfig->mediaPathId();
    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();

    m_animationAssetFileStoreMapper = std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    m_animatorFileStoreMapper = std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(m_animatorFileStoreMapper, m_animationAssetFileStoreMapper);
    m_geometryDataFileStoreMapper = std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    auto geometry_policy = std::make_shared<Enigma::Geometries::GeometryInstallingPolicy>(m_geometryDataFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@" + mediaPathId));
    m_textureFileStoreMapper = std::make_shared<TextureFileStoreMapper>("textures.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(m_textureFileStoreMapper);

    m_primitiveFileStoreMapper = std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    auto primitive_policy = std::make_shared<Enigma::Primitives::PrimitiveRepositoryInstallingPolicy>(m_primitiveFileStoreMapper);

    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto deferred_config = std::make_shared<DeferredRendererServiceConfiguration>();
    deferred_config->sunLightEffect() = EffectMaterialId("fx/DeferredShadingWithShadowSunLightPass");
    deferred_config->sunLightSpatialFlags() |= SpatialShadowFlags::Spatial_ShadowReceiver;
    auto deferred_renderer_policy = std::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, deferred_config);
    //auto scene_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    //auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(m_appConfig->GetDefaultRendererName(), m_appConfig->GetPrimaryTargetName(), scene_render_config);

    m_sceneGraphFileStoreMapper = std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@" + mediaPathId, "lazy_scene.db.txt@" + mediaPathId, "lazied_", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto renderables_policy = std::make_shared<Enigma::Renderables::RenderablesInstallingPolicy>();

    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>();
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(m_appConfig->cameraId(), m_appConfig->cameraDto());

    m_worldMapFileStoreMapper = std::make_shared<WorldMapFileStoreMapper>("world_map.db.txt@" + mediaPathId, "quad_root.db.txt@" + mediaPathId, std::make_shared<DtoJsonGateway>());
    auto world_map_policy = std::make_shared<WorldMapInstallingPolicy>(m_worldMapFileStoreMapper);
    auto terrain_policy = std::make_shared<TerrainInstallingPolicy>();
    auto shadow_map_config = std::make_shared<ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    auto animated_pawn_policy = std::make_shared<AnimatedPawnInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, deferred_renderer_policy, animator_policy, scene_graph_policy, input_handler_policy, game_camera_policy, world_map_policy, game_scene_policy, terrain_policy, shadow_map_policy, animated_pawn_policy, geometry_policy, primitive_policy, renderables_policy, effect_material_source_policy, texture_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('A');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('D');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('S');
    m_inputHandler.lock()->RegisterKeyboardAsyncKey('W');
    m_sceneRenderer = m_graphicMain->getSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->getSystemServiceAs<ShadowMapService>();
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<WorldEditService>(m_graphicMain->getServiceManager(), m_graphicMain->getSystemServiceAs<WorldMapRepository>()));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<TerrainEditService>(m_graphicMain->getServiceManager(), m_textureFileStoreMapper));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<LightEditService>(m_graphicMain->getServiceManager()));
    m_graphicMain->getServiceManager()->registerSystemService(std::make_shared<PawnEditService>(m_graphicMain->getServiceManager()));

    m_sceneGraphFileStoreMapper->subscribeHandlers();
    m_worldMapFileStoreMapper->subscribeHandlers();
    m_textureFileStoreMapper->subscribeHandlers();
}

void EditorAppDelegate::shutdownEngine()
{
    m_sceneGraphFileStoreMapper->unsubscribeHandlers();
    m_worldMapFileStoreMapper->unsubscribeHandlers();
    m_textureFileStoreMapper->unsubscribeHandlers();

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
    EventPublisher::unsubscribe(typeid(PortalSceneRootCreated), m_onSceneRootCreated);
    m_onSceneRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneGraphChanged = nullptr;
    //EventPublisher::unsubscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    //m_onWorldMapCreated = nullptr;
    //EventPublisher::unsubscribe(typeid(CreateWorldMapFailed), m_onCreateWorldFailed);
    //m_onCreateWorldFailed = nullptr;
    //EventPublisher::unsubscribe(typeid(WorldMapDeserialized), m_onWorldMapDeserialized);
    //m_onWorldMapDeserialized = nullptr;
    //EventPublisher::unsubscribe(typeid(DeserializeWorldMapFailed), m_onDeserializeWorldMapFailed);
    //m_onDeserializeWorldMapFailed = nullptr;

    assert(m_graphicMain);
    m_graphicMain->getServiceManager()->shutdownSystemService(TerrainEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(WorldEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(LightEditService::TYPE_RTTI);
    m_graphicMain->getServiceManager()->shutdownSystemService(PawnEditService::TYPE_RTTI);
    m_graphicMain->shutdownRenderEngine();
}

void EditorAppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->frameUpdate();
}

void EditorAppDelegate::prepareRender()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->prepareShadowScene();
    if (!m_sceneRenderer.expired()) m_sceneRenderer.lock()->prepareGameScene();
}

void EditorAppDelegate::renderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->renderShadowScene();
    if (!m_sceneRenderer.expired())
    {
        m_sceneRenderer.lock()->renderGameScene();
        m_sceneRenderer.lock()->flip();
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

void EditorAppDelegate::onRenderEngineInstalled(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderEngineInstalled, IEvent>(e);
    if (!ev) return;
    CommandBus::enqueue(std::make_shared<OutputMessage>("render engine installed"));
    auto root_id = m_appConfig->portalManagementId();
    if (!m_sceneGraphFileStoreMapper->hasSpatial(root_id))
    {
        PortalManagementNodeAssembler root_assembler(root_id);
        root_assembler.asNative(root_id.name() + ".node@" + m_appConfig->mediaPathId());
        m_sceneGraphFileStoreMapper->putSpatial(root_id, root_assembler.toGenericDto());
    }
    CommandBus::enqueue(std::make_shared<CreatePortalSceneRoot>(root_id));
}

void EditorAppDelegate::onSceneRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PortalSceneRootCreated, IEvent>(e);
    if (!ev) return;
    CommandBus::enqueue(std::make_shared<OutputMessage>("portal scene root created : " + ev->root()->id().name()));
    m_sceneRoot = ev->root();
    SceneFlattenTraversal traversal;
    m_sceneRoot.lock()->visitBy(&traversal);
    CommandBus::enqueue(std::make_shared<RefreshSceneGraph>(traversal.GetSpatials()));
}

void EditorAppDelegate::onSceneGraphChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraphChanged, IEvent>(e);
    if (!ev) return;
    if (m_sceneRoot.expired()) return;
    SceneFlattenTraversal traversal;
    m_sceneRoot.lock()->visitBy(&traversal);
    CommandBus::enqueue(std::make_shared<RefreshSceneGraph>(traversal.GetSpatials()));
}

/*void EditorAppDelegate::onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WorldMapCreated, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("world map created : " + ev->name()));
    if (!m_sceneRoot.expired())
    {
        m_sceneRoot.lock()->attachChild(ev->world()->getRoot(), Enigma::MathLib::Matrix4::IDENTITY);
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
        m_sceneRoot.lock()->attachChild(ev->world()->getRoot(), Enigma::MathLib::Matrix4::IDENTITY);
    }
}

void EditorAppDelegate::onDeserializeWorldMapFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<DeserializeWorldMapFailed, IEvent>(e);
    if (!ev) return;
    CommandBus::post(std::make_shared<OutputMessage>("deserialize world map " + ev->name() + " failed : " + ev->error().message()));
}*/

