﻿#include "Animators/AnimatorCommands.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "Controllers/ControllerEvents.h"
#include "DaeParser.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameCommon/AvatarRecipes.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/GameLightCommands.h"
#include "GameCommon/GameSceneCommands.h"
#include "GameCommon/GameSceneEvents.h"
#include "GameCommon/GameSceneService.h"
#include "Rendering/SceneRendererInstallingPolicy.h"
#include "Rendering/DeferredRendering.h"
#include "Rendering/DeferredRenderingConfiguration.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Gateways/DtoJsonGateway.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "GraphicAPIDx11/GraphicAPIDx11.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "SceneGraph/NodeAssembler.h"
#include "SceneGraph/CameraAssembler.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "ViewerAppDelegate.h"
#include "ViewerCommands.h"
#include "ViewerRenderablesFileStoreMapper.h"
#include "FloorReceiverMaker.h"
#include "ViewerTextureFileStoreMapper.h"
#include "ViewerAvatarBaker.h"
#include "Terrain/TerrainInstallingPolicy.h"
#include "CameraConcatenater.h"
#include <memory>

using namespace EnigmaViewer;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;
using namespace Enigma::FileSystem;
using namespace Enigma::Controllers;
using namespace Enigma::Devices;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Animators;
using namespace Enigma::SceneGraph;
using namespace Enigma::GameCommon;
using namespace Enigma::Gateways;
using namespace Enigma::Frameworks;
using namespace Enigma::ShadowMap;
using namespace Enigma::MathLib;
using namespace Enigma::FileStorage;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";
std::string ViewingPawnName = "_ViewingPawn_";

ViewerAppDelegate::ViewerAppDelegate()
{
    m_hasLogFile = false;
    m_graphicMain = nullptr;
    m_hwnd = nullptr;
}

ViewerAppDelegate::~ViewerAppDelegate()
{
}

void ViewerAppDelegate::initialize(IGraphicAPI::APIVersion api_ver, IGraphicAPI::AsyncType useAsyncDevice,
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

    CoInitializeEx(NULL, COINIT_MULTITHREADED);  // for WIC Texture Loader

    installEngine();
}

void ViewerAppDelegate::finalize()
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
    delete FileSystem::instance();

    CoUninitialize();
}

void ViewerAppDelegate::initializeMountPaths()
{
    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        auto dataPath = path / "Data";
        auto mediaPath = path / "../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), "APK_PATH"));
        if (!std::filesystem::exists(dataPath))
        {
            std::filesystem::create_directory(dataPath);
        }
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(dataPath.string(), "DataPath"));
    }
}

void ViewerAppDelegate::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphRootCreated(e); });
    EventPublisher::subscribe(typeid(NodalSceneRootCreated), m_onSceneGraphRootCreated);

    m_loadModelPrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->loadModelPrimitive(c); });
    CommandBus::subscribe(typeid(LoadModelPrimitive), m_loadModelPrimitive);
    m_removeModelPrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->removeModelPrimitive(c); });
    CommandBus::subscribe(typeid(RemoveModelPrimitive), m_removeModelPrimitive);
    m_createAnimatedPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->createAnimatedPawn(c); });
    CommandBus::subscribe(typeid(CreateAnimatedPawn), m_createAnimatedPawn);
    m_loadAnimatedPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->loadAnimatedPawn(c); });
    CommandBus::subscribe(typeid(LoadAnimatedPawn), m_loadAnimatedPawn);
    m_removeAnimatedPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->removeAnimatedPawn(c); });
    CommandBus::subscribe(typeid(RemoveAnimatedPawn), m_removeAnimatedPawn);

    assert(m_graphicMain);

    m_primaryCameraId = SpatialId("camera", Camera::TYPE_RTTI);

    installEngineService();
    installDomainService();
    installUseCaseService();
    installPresentationService();
    installGameCommonService();

    m_graphicMain->installRenderEngine(m_policyList);
    m_inputHandler = m_graphicMain->getSystemServiceAs<Enigma::InputHandlers::InputHandlerService>();
    m_sceneRendering = m_graphicMain->getSystemServiceAs<Enigma::Rendering::SceneRendering>();
    m_shadowMapService = m_graphicMain->getSystemServiceAs<ShadowMapService>();
    m_gameSceneService = m_graphicMain->getSystemServiceAs<GameSceneService>();

    m_primitiveFileStoreMapper->subscribeHandlers();
    m_sceneGraphFileStoreMapper->subscribeHandlers();
    m_textureFileStoreMapper->subscribeHandlers();
    m_viewingPawnId = SpatialId(ViewingPawnName, AnimatedPawn::TYPE_RTTI);
    m_viewingPawnPresentation.subscribeHandlers();
    ViewerAvatarBaker::GetCurrentPawn get_current_pawn = [=]() { return m_viewingPawnPresentation.pawn(); };
    m_avatarBaker.subscribeHandlers(get_current_pawn);
    ViewerAnimationClipCommandHandler::GetCurrentPawn get_current_pawn2 = [=]() { return m_viewingPawnPresentation.pawn(); };
    m_animationClipCommandHandler.registerHandlers(get_current_pawn2);
}

void ViewerAppDelegate::registerMediaMountPaths(const std::string& media_path)
{
}

void ViewerAppDelegate::shutdownEngine()
{
    m_animationClipCommandHandler.unregisterHandlers();
    m_avatarBaker.unsubscribeHandlers();
    m_viewingPawnPresentation.removePawn(m_sceneRootId);
    m_viewingPawnPresentation.unsubscribeHandlers();
    m_sceneRoot = nullptr;

    m_primitiveFileStoreMapper->unsubscribeHandlers();
    m_sceneGraphFileStoreMapper->unsubscribeHandlers();
    m_textureFileStoreMapper->unsubscribeHandlers();

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
    EventPublisher::unsubscribe(typeid(NodalSceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;

    CommandBus::unsubscribe(typeid(LoadModelPrimitive), m_loadModelPrimitive);
    m_loadModelPrimitive = nullptr;
    CommandBus::unsubscribe(typeid(RemoveModelPrimitive), m_removeModelPrimitive);
    m_removeModelPrimitive = nullptr;
    CommandBus::unsubscribe(typeid(CreateAnimatedPawn), m_createAnimatedPawn);
    m_createAnimatedPawn = nullptr;
    CommandBus::unsubscribe(typeid(LoadAnimatedPawn), m_loadAnimatedPawn);
    m_loadAnimatedPawn = nullptr;
    CommandBus::unsubscribe(typeid(RemoveAnimatedPawn), m_removeAnimatedPawn);
    m_removeAnimatedPawn = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void ViewerAppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->frameUpdate();
}

void ViewerAppDelegate::prepareRender()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->prepareShadowScene();
    if ((!m_sceneRendering.expired()) && (!m_gameSceneService.expired()) && (m_gameSceneService.lock()->getSceneCuller()))
    {
        m_sceneRendering.lock()->prepareGameScene(m_gameSceneService.lock()->getSceneCuller()->getVisibleSet());
    }
}

void ViewerAppDelegate::renderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->renderShadowScene();
    if (!m_sceneRendering.expired())
    {
        m_sceneRendering.lock()->renderGameScene();
        m_sceneRendering.lock()->flip();
    }
}

void ViewerAppDelegate::onTimerElapsed()
{
    if (!m_graphicMain) return;

    frameUpdate();

    prepareRender();
    renderFrame();
}

void ViewerAppDelegate::importDaeFile(const std::string& filename)
{
    DaeParser parser(m_geometryDataFileStoreMapper, m_animationAssetFileStoreMapper, m_animatorFileStoreMapper, m_primitiveFileStoreMapper);
    parser.loadDaeFile(filename);
    refreshModelList();
}

void ViewerAppDelegate::saveAnimatedPawn()
{
    if (!m_viewingPawnPresentation.hasPawn()) return;
    if (!m_sceneGraphFileStoreMapper) return;
    m_sceneGraphFileStoreMapper->putSpatial(m_viewingPawnPresentation.presentingPawnId(), SpatialAssembler::assembledAssemblerOf(m_viewingPawnPresentation.pawn())->assemble());
    refreshPawnList();
}

void ViewerAppDelegate::installEngineService()
{
    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    m_policyList.insert(m_policyList.end(), { creating_policy, engine_policy, render_sys_policy });
}

void ViewerAppDelegate::installDomainService()
{
    m_geometryDataFileStoreMapper = std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto geometry_policy = std::make_shared<Enigma::Geometries::GeometryInstallingPolicy>(m_geometryDataFileStoreMapper);
    m_primitiveFileStoreMapper = std::make_shared<ViewerRenderablesFileStoreMapper>("primitives.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto primitive_policy = std::make_shared<Enigma::Primitives::PrimitiveRepositoryInstallingPolicy>(m_primitiveFileStoreMapper);
    m_animationAssetFileStoreMapper = std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@APK_PATH ", std::make_shared<DtoJsonGateway>());
    m_animatorFileStoreMapper = std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(m_animatorFileStoreMapper, m_animationAssetFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    m_textureFileStoreMapper = std::make_shared<ViewerTextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(m_textureFileStoreMapper);

    m_policyList.insert(m_policyList.end(), { geometry_policy, primitive_policy, animator_policy, effect_material_source_policy, texture_policy });
}

void ViewerAppDelegate::installUseCaseService()
{
    m_sceneGraphFileStoreMapper = std::make_shared<ViewerSceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto renderables_policy = std::make_shared<Enigma::Renderables::RenderablesInstallingPolicy>();

    m_policyList.insert(m_policyList.end(), { scene_graph_policy, renderables_policy });
}

void ViewerAppDelegate::installPresentationService()
{
    auto deferred_config = std::make_shared<Enigma::Rendering::DeferredRenderingConfiguration>();
    deferred_config->primaryCameraId(m_primaryCameraId);
    deferred_config->sunLightEffect(EffectMaterialId("fx/DeferredShadingWithShadowSunLightPass"));
    deferred_config->sunLightSpatialFlags(deferred_config->sunLightSpatialFlags() | static_cast<Spatial::SpatialFlags>(SpatialShadowFlags::Spatial_ShadowReceiver));
    auto deferred_renderer_policy = std::make_shared<Enigma::Rendering::DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, deferred_config);
    //auto scene_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    //auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, scene_render_config);

    m_policyList.insert(m_policyList.end(), { deferred_renderer_policy });
}

void ViewerAppDelegate::installGameCommonService()
{
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(m_primaryCameraId, CameraConcatenater(m_primaryCameraId).eyePosition(Enigma::MathLib::Vector3(-5.0f, 5.0f, -5.0f)).lookAt(Enigma::MathLib::Vector3(1.0f, -1.0f, 1.0f)).upVector(Enigma::MathLib::Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Radian(Math::PI / 4.0f)).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).asNative(m_primaryCameraId.name() + ".cam@DataPath").assemble());
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>();
    auto animated_pawn = std::make_shared<AnimatedPawnInstallingPolicy>();
    auto shadow_map_config = std::make_shared<ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    auto terrain_policy = std::make_shared<Enigma::Terrain::TerrainInstallingPolicy>();
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();

    m_policyList.insert(m_policyList.end(), { game_camera_policy, game_scene_policy, animated_pawn, shadow_map_policy, terrain_policy, input_handler_policy });
}

void ViewerAppDelegate::onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e)
{
    m_sceneRootId = SpatialId(SceneRootName, Node::TYPE_RTTI);
    if ((!e) || (e->typeInfo() != typeid(RenderEngineInstalled))) return;
    if (!m_sceneGraphFileStoreMapper->hasSpatial(m_sceneRootId))
    {
        NodeAssembler root_assembler(m_sceneRootId);
        root_assembler.persist(m_sceneRootId.name() + ".node", "DataPath");
        m_sceneGraphFileStoreMapper->putSpatial(m_sceneRootId, root_assembler.assemble());
    }
    CommandBus::enqueue(std::make_shared<CreateNodalSceneRoot>(m_sceneRootId));

    refreshModelList();
    refreshPawnList();
}

void ViewerAppDelegate::onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<NodalSceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->root();
    LightInfo amb_info(LightInfo::LightType::Ambient);
    amb_info.color(Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f));
    SpatialId amb_light_id("amb_lit", Light::TYPE_RTTI);
    FactoryDesc amb_fd = FactoryDesc(Light::TYPE_RTTI).ClaimAsInstanced(amb_light_id.name() + ".light", "DataPath");
    CommandBus::enqueue(std::make_shared<CreateAmbientLight>(m_sceneRootId, amb_light_id, amb_info, amb_fd, PersistenceLevel::Repository));
    LightInfo sun_info(LightInfo::LightType::SunLight);
    sun_info.color(Enigma::MathLib::ColorRGBA(0.6f, 0.6f, 0.6f, 1.0f));
    sun_info.direction(Enigma::MathLib::Vector3(-1.0f, -1.0f, -1.0f));
    SpatialId sun_light_id("sun_lit", Light::TYPE_RTTI);
    FactoryDesc sun_fd = FactoryDesc(Light::TYPE_RTTI).ClaimAsInstanced(sun_light_id.name() + ".light", "DataPath");
    CommandBus::enqueue(std::make_shared<CreateSunLight>(m_sceneRootId, sun_light_id, sun_info, sun_fd, PersistenceLevel::Repository));
    LightInfo pt_info(LightInfo::LightType::Point);
    pt_info.color(Enigma::MathLib::ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f));
    pt_info.range(3.50f);
    pt_info.position(Enigma::MathLib::Vector3(2.0f, 2.0f, 2.0f));
    SpatialId pt_light_id("point_lit", Light::TYPE_RTTI);
    FactoryDesc pt_fd = FactoryDesc(Light::TYPE_RTTI).ClaimAsInstanced(pt_light_id.name() + ".light", "DataPath");
    auto mx = Enigma::MathLib::Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::enqueue(std::make_shared<CreatePointLight>(m_sceneRootId, mx, pt_light_id, pt_info, pt_fd, PersistenceLevel::Repository));
    FloorReceiverMaker::makeFloorReceiver(m_sceneRoot);
}

void ViewerAppDelegate::loadModelPrimitive(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<LoadModelPrimitive, ICommand>(c);
    if (!cmd) return;
    loadModelPrimitive(cmd->name());
}

void ViewerAppDelegate::createAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<CreateAnimatedPawn, ICommand>(c);
    if (!cmd) return;
    m_creatingPawnId = SpatialId(cmd->name(), AnimatedPawn::TYPE_RTTI);
    m_viewingPawnPresentation.presentPawn(m_creatingPawnId, cmd->modelId(), m_sceneRootId);
}

void ViewerAppDelegate::loadAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<LoadAnimatedPawn, ICommand>(c);
    if (!cmd) return;
    loadAnimatedPawn(cmd->name());
}

void ViewerAppDelegate::removeModelPrimitive(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RemoveModelPrimitive, ICommand>(c);
    if (!cmd) return;
    removeModelPrimitive(cmd->name());
}

void ViewerAppDelegate::removeAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RemoveAnimatedPawn, ICommand>(c);
    if (!cmd) return;
    removeAnimatedPawn(cmd->name());
}

void ViewerAppDelegate::refreshModelList()
{
    if (m_primitiveFileStoreMapper)
    {
        CommandBus::enqueue(std::make_shared<RefreshModelPrimitiveList>(m_primitiveFileStoreMapper->modelNames()));
    }
}

void ViewerAppDelegate::loadModelPrimitive(const std::string& model_name)
{
    if (!m_primitiveFileStoreMapper) return;
    auto model_id = m_primitiveFileStoreMapper->modelId(model_name);
    if (!model_id) return;
    m_viewingPawnPresentation.presentPawn(m_viewingPawnId, model_id.value(), m_sceneRootId);
}

void ViewerAppDelegate::removeModelPrimitive(const std::string& model_name)
{
    if (!m_primitiveFileStoreMapper) return;
    auto model_id = m_primitiveFileStoreMapper->modelId(model_name);
    if (!model_id) return;
    m_primitiveFileStoreMapper->removePrimitive(model_id.value());
    refreshModelList();
}

void ViewerAppDelegate::refreshPawnList()
{
    if (m_sceneGraphFileStoreMapper)
    {
        CommandBus::enqueue(std::make_shared<RefreshPawnList>(m_sceneGraphFileStoreMapper->pawnNames()));
    }
}

void ViewerAppDelegate::loadAnimatedPawn(const std::string& pawn_name)
{
    if (!m_sceneGraphFileStoreMapper) return;
    auto pawn_id = m_sceneGraphFileStoreMapper->pawnId(pawn_name);
    if (!pawn_id) return;
    m_viewingPawnPresentation.loadPawn(pawn_id.value(), m_sceneRootId);
}

void ViewerAppDelegate::removeAnimatedPawn(const std::string& pawn_name)
{
    if (!m_sceneGraphFileStoreMapper) return;
    auto pawn_id = m_sceneGraphFileStoreMapper->pawnId(pawn_name);
    if (!pawn_id) return;
    m_sceneGraphFileStoreMapper->removeSpatial(pawn_id.value());
    refreshPawnList();
}
