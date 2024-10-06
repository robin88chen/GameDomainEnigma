#include "DeferredRenderingTest.h"
#include "CameraConcatenater.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/NodeAssembler.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "Frameworks/menew_make_shared.hpp"
#include "GameCommon/GameSceneService.h"
#include "GameCommon/GameSceneEvents.h"
#include "GameCommon/GameLightCommands.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Geometries/GeometryData.h"
#include "Geometries/StandardGeometryAssemblers.h"
#include "Renderables/MeshPrimitive.h"
#include "Primitives/PrimitiveQueries.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/Light.h"
#include "Controllers/ControllerEvents.h"
#include "Rendering/DeferredRenderingConfiguration.h"
#include "Rendering/SceneRendererInstallingPolicy.h"
#include "GameCommon/GameSceneCommands.h"
#include "Renderables/MeshPrimitiveAssembler.h"
#include "SceneGraph/PawnAssembler.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/ApplicationBridge.h"
#endif

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
using namespace Enigma::FileStorage;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::Renderables;
using namespace Enigma::Animators;
using namespace Enigma::Rendering;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";

DeferredRenderingTest::DeferredRenderingTest(const std::string app_name) : AppDelegate(app_name)
{
}

DeferredRenderingTest::~DeferredRenderingTest()
{
}

void DeferredRenderingTest::initializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        auto dataPath = path / "Data";
        auto mediaPath = path / "../../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
        if (!std::filesystem::exists(dataPath))
        {
            std::filesystem::create_directory(dataPath);
        }
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(dataPath.string(), "DataPath"));
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    if (FileSystem::instance())
    {
        FileSystem::instance()->addMountPath(std::make_shared<AndroidMountPath>("", MediaPathName));

        std::string data_path = Enigma::Platforms::AndroidBridge::getDataFilePath();
        char s[2048];
        memset(s, 0, 2048);
        memcpy(s, data_path.c_str(), data_path.size());
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(data_path, "DataPath"));
    }
#endif
}

void DeferredRenderingTest::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphRootCreated(e); });
    EventPublisher::subscribe(typeid(NodalSceneRootCreated), m_onSceneGraphRootCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    m_sceneGraphFileStoreMapper = std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", "lazied_", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto camera_id = SpatialId("camera", Camera::TYPE_RTTI);
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(camera_id,
        CameraConcatenater(camera_id).eyePosition(Vector3(-5.0f, 5.0f, -5.0f)).lookAt(Vector3(1.0f, -1.0f, 1.0f)).upVector(Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Radian(Math::PI / 4.0f)).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).asNative(camera_id.name() + ".cam@DataPath").assemble());
    auto deferred_config = std::make_unique<DeferredRenderingConfiguration>();
    deferred_config->ambientEffect(EffectMaterialId("fx/DeferredShadingAmbientPass"));
    deferred_config->sunLightEffect(EffectMaterialId("fx/DeferredShadingSunLightPass"));
    deferred_config->lightVolumeEffect(EffectMaterialId("fx/DeferredShadingLightVolume"));
    deferred_config->deferredRendererTechniqueName("DeferredRenderer");
    deferred_config->visualTechniqueNameForCameraDefault("Default");
    deferred_config->visualTechniqueNameForCameraInside("Inside");
    deferred_config->gbufferTargetName("gbuffer_target");
    deferred_config->gbufferSurfaceName("gbuffer_surface");
    deferred_config->gbufferDepthName("gbuffer_depth");
    deferred_config->gbufferNormalSemantic("GBufferNormalMap");
    deferred_config->gbufferDiffuseSemantic("GBufferDiffuseMap");
    deferred_config->gbufferSpecularSemantic("GBufferSpecularMap");
    deferred_config->gbufferDepthSemantic("GBufferDepthMap");
    deferred_config->primaryCameraId(camera_id);
    auto deferred_renderer_policy = stdext::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, std::move(deferred_config));
    m_sceneRootId = SpatialId(SceneRootName, Node::TYPE_RTTI);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>();
    //auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, input_handler_policy, game_camera_policy, deferred_renderer_policy, game_scene_policy, texture_policy, renderables_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
#if TARGET_PLATFORM == PLATFORM_ANDROID
    ApplicationBridge::InitInputHandler(input_handler_policy->GetInputHandler());
#endif
    m_sceneRendering = m_graphicMain->getSystemServiceAs<SceneRendering>();
    m_sceneGraphRepository = m_graphicMain->getSystemServiceAs<SceneGraphRepository>();
    m_gameSceneService = m_graphicMain->getSystemServiceAs<GameSceneService>();
}

void DeferredRenderingTest::shutdownEngine()
{
    m_sceneRoot = nullptr;
    m_pawn = nullptr;

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
    EventPublisher::unsubscribe(typeid(NodalSceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void DeferredRenderingTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if ((!m_sceneRendering.expired()) && (!m_gameSceneService.expired()) && (m_gameSceneService.lock()->getSceneCuller())) m_sceneRendering.lock()->prepareGameScene(m_gameSceneService.lock()->getSceneCuller()->getVisibleSet());
}

void DeferredRenderingTest::renderFrame()
{
    if (!m_sceneRendering.expired())
    {
        m_sceneRendering.lock()->renderGameScene();
        m_sceneRendering.lock()->flip();
    }
}

void DeferredRenderingTest::onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e)
{
    if ((!e) || (e->typeInfo() != typeid(RenderEngineInstalled))) return;
    if (!m_sceneGraphFileStoreMapper->hasSpatial(m_sceneRootId))
    {
        NodeAssembler root_assembler(m_sceneRootId);
        root_assembler.persist(m_sceneRootId.name() + ".node", "DataPath");
        m_sceneGraphFileStoreMapper->putSpatial(m_sceneRootId, root_assembler.assemble());
    }
    CommandBus::enqueue(std::make_shared<CreateNodalSceneRoot>(m_sceneRootId));
}

void DeferredRenderingTest::onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<NodalSceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->root();
    LightInfo ambient_light_info(LightInfo::LightType::Ambient);
    ambient_light_info.color(ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f));
    std::make_shared<CreateAmbientLight>(m_sceneRootId, SpatialId("amb_lit", Light::TYPE_RTTI), ambient_light_info, FactoryDesc(Light::TYPE_RTTI), PersistenceLevel::Repository)->execute();
    LightInfo sun_light_info(LightInfo::LightType::SunLight);
    sun_light_info.color(ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f));
    sun_light_info.direction(Vector3(-1.0f, -1.0f, -1.0f));
    std::make_shared<CreateSunLight>(m_sceneRootId, SpatialId("sun_lit", Light::TYPE_RTTI), sun_light_info, FactoryDesc(Light::TYPE_RTTI), PersistenceLevel::Repository)->execute();
    auto mx = Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    LightInfo point_light_info(LightInfo::LightType::Point);
    point_light_info.color(ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f));
    point_light_info.position(Vector3(2.0f, 2.0f, 2.0f));
    point_light_info.range(3.50f);
    std::make_shared<CreatePointLight>(m_sceneRootId, mx, SpatialId("point_lit", Light::TYPE_RTTI), point_light_info, FactoryDesc(Light::TYPE_RTTI), PersistenceLevel::Repository)->execute();
    createCubePawn();
}

void DeferredRenderingTest::createCubePawn()
{
    auto cube_geo_id = GeometryId("cube");
    auto cube_geo = GeometryData::queryGeometryData(cube_geo_id);
    if (!cube_geo)
    {
        cube_geo = CubeAssembler(cube_geo_id).facedCube(Vector3::ZERO, Vector3(1.0f, 1.0f, 1.0f)).facedNormal().facedTextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f)).asAsset(cube_geo_id.name(), cube_geo_id.name() + ".geo", "DataPath").constitute();
    }
    auto cube_mesh_id = PrimitiveId("cube_mesh", MeshPrimitive::TYPE_RTTI);
    auto cube_mesh = std::make_shared<QueryPrimitive>(cube_mesh_id)->dispatch();
    if (!cube_mesh)
    {
        MeshPrimitiveAssembler mesh_assembler(cube_mesh_id);
        mesh_assembler.geometryId(cube_geo_id);
        mesh_assembler.addMaterial(std::make_shared<PrimitiveMaterial>(EffectMaterialId("fx/default_textured_mesh_effect"), EffectTextureMap({ {TextureId("image/du011"), std::nullopt, "DiffuseMap"} })));
        mesh_assembler.visualTechnique("Default");
        mesh_assembler.renderListID(Renderer::RenderListID::Scene);
        mesh_assembler.asNative(cube_mesh_id.name() + ".mesh@DataPath");
        cube_mesh = std::make_shared<RequestPrimitiveConstitution>(cube_mesh_id, mesh_assembler.assemble())->dispatch();
    }
    m_cubeId = SpatialId("cube_pawn", Pawn::TYPE_RTTI);
    m_pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(m_cubeId)->dispatch());
    if (!m_pawn)
    {
        PawnAssembler assembler(m_cubeId);
        assembler.primitiveId(cube_mesh_id);
        assembler.localTransform(Matrix4::IDENTITY);
        assembler.topLevel(true);
        m_pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(m_cubeId, assembler.assemble())->dispatch());
    }
    if ((m_sceneRoot) && (m_pawn))
    {
        m_sceneRoot->attachChild(m_pawn, Matrix4::IDENTITY);
    }
}
