#include "Animators/AnimatorCommands.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "Controllers/ControllerEvents.h"
#include "DaeParser.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"
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
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EffectDtoHelper.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Gateways/DtoJsonGateway.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "GeometryCreationHelper.h"
#include "GraphicAPIDx11/GraphicAPIDx11.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Primitives/Primitive.h"
#include "Primitives/PrimitiveQueries.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "ViewerAppDelegate.h"
#include "ViewerCommands.h"
#include "ViewerRenderablesFileStoreMapper.h"
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
using namespace Enigma::Renderables;
using namespace Enigma::Primitives;
using namespace Enigma::Geometries;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";
std::string ViewingPawnName = "_ViewingPawn_";
std::string FloorReceiverName = "_Floor_";

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

    m_changeMeshTexture = std::make_shared<CommandSubscriber>([=](auto c) { this->changeMeshTexture(c); });
    CommandBus::subscribe(typeid(ChangeMeshTexture), m_changeMeshTexture);
    m_addAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->addAnimationClip(c); });
    CommandBus::subscribe(typeid(AddAnimationClip), m_addAnimationClip);
    m_deleteAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->deleteAnimationClip(c); });
    CommandBus::subscribe(typeid(DeleteAnimationClip), m_deleteAnimationClip);
    m_playAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->playAnimationClip(c); });
    CommandBus::subscribe(typeid(PlayAnimationClip), m_playAnimationClip);
    m_changeAnimationTimeValue = std::make_shared<CommandSubscriber>([=](auto c) { this->changeAnimationTimeValue(c); });
    CommandBus::subscribe(typeid(ChangeAnimationTimeValue), m_changeAnimationTimeValue);
    m_loadModelPrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->loadModelPrimitive(c); });
    CommandBus::subscribe(typeid(LoadModelPrimitive), m_loadModelPrimitive);
    m_createAnimatedPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->createAnimatedPawn(c); });
    CommandBus::subscribe(typeid(CreateAnimatedPawn), m_createAnimatedPawn);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    m_geometryDataFileStoreMapper = std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(m_geometryDataFileStoreMapper);
    m_primitiveFileStoreMapper = std::make_shared<ViewerRenderablesFileStoreMapper>("primitives.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(m_primitiveFileStoreMapper);
    m_animationAssetFileStoreMapper = std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@APK_PATH ", std::make_shared<DtoJsonGateway>());
    m_animatorFileStoreMapper = std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(m_animatorFileStoreMapper, m_animationAssetFileStoreMapper);
    m_sceneGraphFileStoreMapper = std::make_shared<ViewerSceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto camera_id = SpatialId("camera", Camera::TYPE_RTTI);
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(camera_id, CameraAssembler(camera_id).eyePosition(Enigma::MathLib::Vector3(-5.0f, 5.0f, -5.0f)).lookAt(Enigma::MathLib::Vector3(1.0f, -1.0f, 1.0f)).upDirection(Enigma::MathLib::Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Enigma::MathLib::Math::PI / 4.0f).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).asNative(camera_id.name() + ".cam@DataPath").toCameraDto().toGenericDto());
    auto deferred_config = std::make_shared<DeferredRendererServiceConfiguration>();
    deferred_config->sunLightEffect() = EffectMaterialId("fx/DeferredShadingWithShadowSunLightPass");
    deferred_config->sunLightSpatialFlags() |= SpatialShadowFlags::Spatial_ShadowReceiver;
    auto deferred_renderer_policy = std::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, deferred_config);
    //auto scene_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    //auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, scene_render_config);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>();
    auto animated_pawn = std::make_shared<AnimatedPawnInstallingPolicy>();
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    auto shadow_map_config = std::make_shared<ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, deferred_renderer_policy /*scene_renderer_policy*/, game_scene_policy, animated_pawn, game_light_policy, shadow_map_policy, geometry_policy, primitive_policy,
        effect_material_source_policy, texture_policy, renderables_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRenderer = m_graphicMain->getSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->getSystemServiceAs<ShadowMapService>();

    m_primitiveFileStoreMapper->subscribeHandlers();
    m_viewingPawnId = SpatialId(ViewingPawnName, AnimatedPawn::TYPE_RTTI);
    m_viewingPawnPresentation.subscribeHandlers();
}

void ViewerAppDelegate::registerMediaMountPaths(const std::string& media_path)
{
}

void ViewerAppDelegate::shutdownEngine()
{
    m_viewingPawnPresentation.removePawn(m_sceneRootId);
    m_viewingPawnPresentation.unsubscribeHandlers();
    m_pawn = nullptr;
    m_sceneRoot = nullptr;
    m_floor = nullptr;

    m_primitiveFileStoreMapper->unsubscribeHandlers();

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
    EventPublisher::unsubscribe(typeid(NodalSceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;

    CommandBus::unsubscribe(typeid(ChangeMeshTexture), m_changeMeshTexture);
    m_changeMeshTexture = nullptr;
    CommandBus::unsubscribe(typeid(AddAnimationClip), m_addAnimationClip);
    m_addAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(DeleteAnimationClip), m_deleteAnimationClip);
    m_deleteAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(PlayAnimationClip), m_playAnimationClip);
    m_playAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(ChangeAnimationTimeValue), m_changeAnimationTimeValue);
    m_changeAnimationTimeValue = nullptr;
    CommandBus::unsubscribe(typeid(LoadModelPrimitive), m_loadModelPrimitive);
    m_loadModelPrimitive = nullptr;
    CommandBus::unsubscribe(typeid(CreateAnimatedPawn), m_createAnimatedPawn);
    m_createAnimatedPawn = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void ViewerAppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->frameUpdate();
}

void ViewerAppDelegate::prepareRender()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->prepareShadowScene();
    if (!m_sceneRenderer.expired()) m_sceneRenderer.lock()->prepareGameScene();
}

void ViewerAppDelegate::renderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->renderShadowScene();
    if (!m_sceneRenderer.expired())
    {
        m_sceneRenderer.lock()->renderGameScene();
        m_sceneRenderer.lock()->flip();
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

/*void ViewerAppDelegate::savePawnFile(const std::filesystem::path& filepath)
{
    if (!m_pawn) return;
    auto pawn_dto = m_pawn->serializeDto();
    pawn_dto.asTopLevel(true);
    std::string json = std::make_shared<DtoJsonGateway>()->serialize(std::vector<GenericDto>{pawn_dto});
    IFilePtr iFile = FileSystem::instance()->openFile(filepath.generic_string(), write | openAlways | binary);
    iFile->write(0, convert_to_buffer(json));
    FileSystem::instance()->closeFile(iFile);
}

void ViewerAppDelegate::loadPawnFile(const std::filesystem::path& filepath)
{
    if (m_pawn)
    {
        m_pawn->detachFromParent();
        m_pawn = nullptr;
    }
    IFilePtr iFile = FileSystem::instance()->openFile(filepath.generic_string(), read | binary);
    size_t file_size = iFile->size();

    auto read_buf = iFile->read(0, file_size);
    FileSystem::instance()->closeFile(iFile);
    auto dtos = std::make_shared<DtoJsonGateway>()->deserialize(convert_to_string(read_buf.value(), file_size));
    CommandBus::post(std::make_shared<BuildSceneGraph>(ViewingPawnName, dtos));
}*/

void ViewerAppDelegate::onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e)
{
    m_sceneRootId = SpatialId(SceneRootName, Node::TYPE_RTTI);
    if ((!e) || (e->typeInfo() != typeid(RenderEngineInstalled))) return;
    if (!m_sceneGraphFileStoreMapper->hasSpatial(m_sceneRootId))
    {
        NodeAssembler root_assembler(m_sceneRootId);
        root_assembler.asNative(m_sceneRootId.name() + ".node@DataPath");
        m_sceneGraphFileStoreMapper->putSpatial(m_sceneRootId, root_assembler.toGenericDto());
    }
    CommandBus::post(std::make_shared<CreateNodalSceneRoot>(m_sceneRootId));

    refreshModelList();
}

void ViewerAppDelegate::onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<NodalSceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->root();
    CommandBus::post(std::make_shared<CreateAmbientLight>(m_sceneRootId, SpatialId("amb_lit", Light::TYPE_RTTI), Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f)));
    CommandBus::post(std::make_shared<CreateSunLight>(m_sceneRootId, SpatialId("sun_lit", Light::TYPE_RTTI), Enigma::MathLib::Vector3(-1.0, -1.0, -1.0), Enigma::MathLib::ColorRGBA(0.6f, 0.6f, 0.6f, 1.0f)));
    auto mx = Enigma::MathLib::Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::post(std::make_shared<CreatePointLight>(m_sceneRootId, mx, SpatialId("point_lit", Light::TYPE_RTTI), Enigma::MathLib::Vector3(2.0f, 2.0f, 2.0f), Enigma::MathLib::ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f), 3.50f));
    createFloorReceiver();
}

void ViewerAppDelegate::changeMeshTexture(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeMeshTexture, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    TextureMappingDto tex_dto;
    tex_dto.textureId() = cmd->textureId();
    tex_dto.semantic() = "DiffuseMap";
    auto recipe = std::make_shared<ChangeAvatarTexture>(cmd->meshName(), tex_dto);
    m_pawn->addAvatarRecipe(recipe);
    m_pawn->bakeAvatarRecipes();
}

void ViewerAppDelegate::addAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<AddAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    if (auto act_clip = m_pawn->animationClipMap().findAnimationClip(cmd->name()); !act_clip)
    {
        AnimationClipMap::AnimClip act_clip_new(cmd->name(), cmd->clip());
        m_pawn->animationClipMap().insertClip(act_clip_new);
    }
    else
    {
        act_clip.value().get().changeClip(cmd->clip());
    }
}

void ViewerAppDelegate::deleteAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<DeleteAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    m_pawn->animationClipMap().removeClip(cmd->name());
}

void ViewerAppDelegate::playAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<PlayAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    m_pawn->playAnimation(cmd->name());
}

void ViewerAppDelegate::changeAnimationTimeValue(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeAnimationTimeValue, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    bool isNameChanged = false;
    if ((m_pawn->animationClipMap().findAnimationClip(cmd->oldName()))
        && (cmd->oldName() != cmd->newName()))
    {
        isNameChanged = true;
    }

    if (!isNameChanged)
    {
        if (auto act_clip = m_pawn->animationClipMap().findAnimationClip(cmd->newName()); act_clip)
        {
            act_clip.value().get().changeClip(cmd->clip());
        }
    }
    else
    {
        m_pawn->animationClipMap().removeClip(cmd->oldName());
        Enigma::GameCommon::AnimationClipMap::AnimClip act_clip_new(cmd->newName(), cmd->clip());
        m_pawn->animationClipMap().insertClip(act_clip_new);
    }
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
}

void ViewerAppDelegate::refreshModelList()
{
    if (m_primitiveFileStoreMapper)
    {
        CommandBus::post(std::make_shared<RefreshModelPrimitiveList>(m_primitiveFileStoreMapper->modelNames()));
    }
}

void ViewerAppDelegate::loadModelPrimitive(const std::string& model_name)
{
    if (!m_primitiveFileStoreMapper) return;
    auto model_id = m_primitiveFileStoreMapper->modelId(model_name);
    if (!model_id) return;
    m_viewingPawnPresentation.presentPawn(m_viewingPawnId, model_id.value(), m_sceneRootId);
}

void ViewerAppDelegate::createFloorReceiver()
{
    m_floorGeometryId = GeometryId("floor");
    auto geometry = GeometryCreationHelper::createSquareXZQuad(m_floorGeometryId);
    m_floorPawnId = SpatialId(FloorReceiverName, Pawn::TYPE_RTTI);
    PawnAssembler pawn_assembler(m_floorPawnId);
    MeshPrimitiveDto mesh_dto;
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.textureMapping(TextureId("image/du011"), std::nullopt, "DiffuseMap");
    m_floorMeshId = PrimitiveId("floor_mesh", MeshPrimitive::TYPE_RTTI);
    mesh_dto.id() = m_floorMeshId;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(m_floorMeshId.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(EffectMaterialId("fx/default_textured_mesh_effect"));
    mesh_dto.textureMaps().emplace_back(tex_dto.toGenericDto());
    mesh_dto.geometryId() = m_floorGeometryId;
    mesh_dto.renderListID() = Enigma::Renderer::Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    auto mesh = std::make_shared<RequestPrimitiveConstitution>(m_floorMeshId, mesh_dto.toGenericDto(), Enigma::Primitives::PersistenceLevel::Repository)->dispatch();
    pawn_assembler.primitive(m_floorMeshId);
    pawn_assembler.spatial().localTransform(Matrix4::IDENTITY).topLevel(true).spatialFlags(SpatialShadowFlags::Spatial_ShadowReceiver);
    m_floor = std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(m_floorPawnId, pawn_assembler.toGenericDto(), Enigma::SceneGraph::PersistenceLevel::Repository)->dispatch());
    if (m_sceneRoot) m_sceneRoot->attachChild(m_floor, Matrix4::IDENTITY);
}
