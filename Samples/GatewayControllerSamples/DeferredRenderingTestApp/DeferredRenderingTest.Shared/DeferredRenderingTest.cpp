#include "DeferredRenderingTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
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
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "Frameworks/menew_make_shared.hpp"
#include "GameCommon/SceneRendererService.h"
#include "GameCommon/GameSceneService.h"
#include "GameCommon/DeferredRendererServiceConfiguration.h"
#include "GameCommon/GameSceneEvents.h"
#include "GameCommon/GameLightCommands.h"
#include "GameEngine/EffectDtoHelper.h"
#include "SceneGraph/SceneGraphDtos.h"
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
#include "SceneGraph/SceneGraphAssemblers.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Geometries/GeometryData.h"
#include "Geometries/StandardGeometryAssemblers.h"
#include "Renderables/MeshPrimitive.h"
#include "Primitives/PrimitiveQueries.h"
#include "Renderables/RenderablePrimitiveAssembler.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/CameraFrustumDtos.h"
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
        auto mediaPath = path / "../../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(path.string(), "DataPath"));
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
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphRootCreated(e); });
    EventPublisher::subscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onPawnPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(std::make_shared<JsonFileDtoDeserializer>(), std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto camera_id = SpatialId("camera", Camera::TYPE_RTTI);
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(camera_id,
        CameraAssembler(camera_id).eyePosition(Vector3(-5.0f, 5.0f, -5.0f)).lookAt(Vector3(1.0f, -1.0f, 1.0f)).upDirection(Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Math::PI / 4.0f).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).toCameraDto().toGenericDto());
    auto deferred_config = std::make_unique<DeferredRendererServiceConfiguration>();
    deferred_config->ambientEffect() = EffectMaterialId("fx/DeferredShadingAmbientPass");
    deferred_config->sunLightEffect() = EffectMaterialId("fx/DeferredShadingSunLightPass");
    deferred_config->lightVolumeEffect() = EffectMaterialId("fx/DeferredShadingLightVolume");
    deferred_config->deferredRendererTechniqueName() = "DeferredRenderer";
    deferred_config->visualTechniqueNameForCameraDefault() = "Default";
    deferred_config->visualTechniqueNameForCameraInside() = "Inside";
    deferred_config->gbufferTargetName() = "gbuffer_target";
    deferred_config->gbufferSurfaceName() = "gbuffer_surface";
    deferred_config->gbufferDepthName() = "gbuffer_depth";
    deferred_config->gbufferNormalSemantic() = "GBufferNormalMap";
    deferred_config->gbufferDiffuseSemantic() = "GBufferDiffuseMap";
    deferred_config->gbufferSpecularSemantic() = "GBufferSpecularMap";
    deferred_config->gbufferDepthSemantic() = "GBufferDepthMap";
    auto deferred_renderer_policy = stdext::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, std::move(deferred_config));
    m_sceneRootId = SpatialId(SceneRootName, Node::TYPE_RTTI);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(m_sceneRootId, PortalManagementName);
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, input_handler_policy, game_camera_policy, deferred_renderer_policy, game_scene_policy, game_light_policy, texture_policy, renderables_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
#if TARGET_PLATFORM == PLATFORM_ANDROID
    ApplicationBridge::InitInputHandler(input_handler_policy->GetInputHandler());
#endif
    m_sceneRendererService = m_graphicMain->getSystemServiceAs<SceneRendererService>();
}

void DeferredRenderingTest::shutdownEngine()
{
    m_sceneRoot = nullptr;
    m_pawn = nullptr;

    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void DeferredRenderingTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->prepareGameScene();
}

void DeferredRenderingTest::renderFrame()
{
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->renderGameScene();
        m_sceneRendererService.lock()->flip();
    }
}

void DeferredRenderingTest::onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
    CommandBus::post(std::make_shared<CreateAmbientLight>(m_sceneRootId, SpatialId("amb_lit", Light::TYPE_RTTI), Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f)));
    CommandBus::post(std::make_shared<CreateSunLight>(m_sceneRootId, SpatialId("sun_lit", Light::TYPE_RTTI), Enigma::MathLib::Vector3(-1.0f, -1.0f, -1.0f), Enigma::MathLib::ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
    auto mx = Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::post(std::make_shared<CreatePointLight>(m_sceneRootId, mx, SpatialId("point_lit", Light::TYPE_RTTI), Vector3(2.0f, 2.0f, 2.0f), ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f), 3.50f));
    createCubePawn();
}

void DeferredRenderingTest::onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    if (ev->GetSceneGraphId() != "cube_pawn") return;
    m_pawn = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
    if (m_sceneRoot) m_sceneRoot->attachChild(m_pawn, Matrix4::IDENTITY);
}

void DeferredRenderingTest::onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    ev->pawn()->getPrimitive()->selectVisualTechnique("Default");
}

void DeferredRenderingTest::createCubePawn()
{
    auto cube_geo_id = GeometryId("cube");
    auto cube_geo = GeometryData::queryGeometryData(cube_geo_id);
    if (!cube_geo)
    {
        cube_geo = CubeAssembler(cube_geo_id).facedCube(Vector3::ZERO, Vector3(1.0f, 1.0f, 1.0f)).facedNormal().facedTextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f)).asAsset(cube_geo_id.name(), cube_geo_id.name() + ".geo", "DataPath").constitute(Enigma::Geometries::PersistenceLevel::Repository);
    }
    auto cube_mesh_id = PrimitiveId("cube_mesh", MeshPrimitive::TYPE_RTTI);
    auto cube_mesh = std::make_shared<QueryPrimitive>(cube_mesh_id)->dispatch();
    if (!cube_mesh)
    {
        cube_mesh = MeshPrimitiveAssembler(cube_mesh_id).geometryId(cube_geo_id).effect(EffectMaterialId("fx/default_textured_mesh_effect")).textureMap(EffectTextureMapDtoHelper().textureMapping(TextureId("image/du011"), std::nullopt, "DiffuseMap")).visualTechnique("Default").renderListID(Renderer::RenderListID::Scene).asNative(cube_mesh_id.name() + ".mesh@DataPath").constitute(Enigma::Primitives::PersistenceLevel::Repository);
    }
    m_cubeId = SpatialId("cube_pawn", Pawn::TYPE_RTTI);
    m_pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(m_cubeId)->dispatch());
    if (!m_pawn)
    {
        m_pawn = PawnAssembler(m_cubeId).primitive(cube_mesh_id).localTransform(Matrix4::IDENTITY).topLevel(true).constitute(Enigma::SceneGraph::PersistenceLevel::Repository);
    }
    if ((m_sceneRoot) && (m_pawn))
    {
        m_sceneRoot->attachChild(m_pawn, Matrix4::IDENTITY);
    }
}
