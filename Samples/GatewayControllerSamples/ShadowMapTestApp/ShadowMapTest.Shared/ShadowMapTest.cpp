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
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "ShadowMap/ShadowMapService.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "Frameworks/EventPublisher.h"
#include "GameCommon/GameSceneEvents.h"
#include "Frameworks/CommandBus.h"
#include "GameCommon/GameLightCommands.h"
#include "GameEngine/EffectDtoHelper.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "ShadowMap/SpatialShadowFlags.h"
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
#include "Geometries/StandardGeometryHelper.h"
#include "Primitives/PrimitiveQueries.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Renderables/RenderablePrimitiveHelper.h"
#include "SceneGraph/SceneGraphHelper.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/ApplicationBridge.h"
#endif

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
using namespace Enigma::Frameworks;
using namespace Enigma::FileStorage;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::Renderables;

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

void ShadowMapTest::initializeMountPaths()
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

void ShadowMapTest::installEngine()
{
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphRootCreated(e); });
    EventPublisher::subscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
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
    auto input_handler_policy = std::make_shared<InputHandlerInstallingPolicy>();
    auto camera_id = SpatialId("camera", Camera::TYPE_RTTI);
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(camera_id,
        CameraDtoHelper(camera_id).eyePosition(Vector3(-10.0f, 10.0f, -10.0f)).lookAt(Vector3(1.0f, -1.0f, 1.0f)).upDirection(Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Math::PI / 4.0f).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).toGenericDto());
    auto shadow_map_config = std::make_unique<ShadowMapServiceConfiguration>();
    auto shadowmap_policy = std::make_shared<ShadowMapInstallingPolicy>(ShadowMapRendererName, ShadowMapTargetName, std::move(shadow_map_config));
    auto primary_render_config = std::make_unique<SceneRendererServiceConfiguration>();
    auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, std::move(primary_render_config));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(SceneRootName, PortalManagementName);
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy,
        effect_material_source_policy, input_handler_policy, game_camera_policy, shadowmap_policy, scene_renderer_policy, game_scene_policy, game_light_policy, texture_policy, renderables_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
#if TARGET_PLATFORM == PLATFORM_ANDROID
    ApplicationBridge::InitInputHandler(input_handler_policy->GetInputHandler());
#endif
    m_sceneRendererService = m_graphicMain->getSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->getSystemServiceAs<ShadowMapService>();
}

void ShadowMapTest::shutdownEngine()
{
    m_sceneRoot = nullptr;
    m_floor = nullptr;
    m_cube = nullptr;

    EventPublisher::unsubscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void ShadowMapTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->prepareShadowScene();
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->prepareGameScene();
}

void ShadowMapTest::renderFrame()
{
    if (!m_shadowMapService.expired())
    {
        m_shadowMapService.lock()->renderShadowScene();
    }
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->renderGameScene();
        m_sceneRendererService.lock()->flip();
    }
}

void ShadowMapTest::onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
    CommandBus::post(std::make_shared<CreateAmbientLight>(ev->GetSceneRoot()->getSpatialName(), "amb_lit", Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f)));
    CommandBus::post(std::make_shared<CreateSunLight>(ev->GetSceneRoot()->getSpatialName(), "sun_lit", Enigma::MathLib::Vector3(-1.0f, -1.0f, -1.0f), Enigma::MathLib::ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
    auto mx = Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::post(std::make_shared<CreatePointLight>(ev->GetSceneRoot()->getSpatialName(), mx, "point_lit", Vector3(2.0f, 2.0f, 2.0f), ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f), 3.50f));

    createFloorReceiver();
    createCubePawn();
}

void ShadowMapTest::onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    if (ev->GetSceneGraphId() == "floor_receiver")
    {
        m_floor = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
        if (m_sceneRoot) m_sceneRoot->attachChild(m_floor, Matrix4::IDENTITY);
    }
    else if (ev->GetSceneGraphId() == "cube_pawn")
    {
        m_cube = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
        Matrix4 mx = Matrix4::MakeTranslateTransform(0.0f, 1.2f, 0.0f);
        if (m_sceneRoot) m_sceneRoot->attachChild(m_cube, mx);
    }
}

void ShadowMapTest::onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->pawn() == m_floor)
    {
        ev->pawn()->getPrimitive()->selectVisualTechnique("ShadowMapReceiver");
    }
    else
    {
        ev->pawn()->getPrimitive()->selectVisualTechnique("Default");
    }
}

void ShadowMapTest::createFloorReceiver()
{
    auto floor_geo_id = GeometryId("floor");
    auto floor_geo = GeometryData::queryGeometryData(floor_geo_id);
    if (!floor_geo)
    {
        SquareQuadHelper floor_helper(floor_geo_id);
        floor_geo = floor_helper.xzQuad(Vector3(-5.0f, 0.0f, -5.0f), Vector3(5.0f, 0.0f, 5.0f)).normal().textureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f)).asAsset(floor_geo_id.name(), floor_geo_id.name() + ".geo", "DataPath").constitute(Enigma::Geometries::PersistenceLevel::Repository);
    }
    auto floor_mesh_id = PrimitiveId("floor_mesh", MeshPrimitive::TYPE_RTTI);
    auto floor_mesh = std::make_shared<QueryPrimitive>(floor_mesh_id)->dispatch();
    if (!floor_mesh)
    {
        MeshPrimitiveHelper mesh_helper(floor_mesh_id);
        floor_mesh = mesh_helper.geometryId(floor_geo_id).effect(EffectMaterialId("fx/default_textured_mesh_effect")).textureMap(EffectTextureMapDtoHelper().textureMapping(TextureId("image/du011"), std::nullopt, "DiffuseMap")).visualTechnique("Default").renderListID(Renderer::RenderListID::Scene).asNative(floor_mesh_id.name() + ".mesh@DataPath").constitute(Enigma::Primitives::PersistenceLevel::Repository);
    }
    m_floorId = SpatialId("floor_receiver", Pawn::TYPE_RTTI);
    m_floor = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(m_floorId)->dispatch());
    if (!m_floor)
    {
        PawnHelper pawn_helper(m_floorId);
        m_floor = pawn_helper.primitive(floor_mesh_id).localTransform(Matrix4::IDENTITY).topLevel(true).spatialFlags(SpatialShadowFlags::Spatial_ShadowReceiver).constitute(Enigma::SceneGraph::PersistenceLevel::Repository);
    }
    if ((m_sceneRoot) && (m_floor)) m_sceneRoot->attachChild(m_floor, Matrix4::IDENTITY);
    if (m_floor) m_floor->getPrimitive()->selectVisualTechnique("ShadowMapReceiver");
}

void ShadowMapTest::createCubePawn()
{
    auto cube_geo_id = GeometryId("cube");
    auto cube_geo = GeometryData::queryGeometryData(cube_geo_id);
    if (!cube_geo)
    {
        CubeHelper cube_helper(cube_geo_id);
        cube_geo = cube_helper.facedCube(Vector3::ZERO, Vector3(1.0f, 1.0f, 1.0f)).facedNormal().facedTextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f)).asAsset(cube_geo_id.name(), cube_geo_id.name() + ".geo", "DataPath").constitute(Enigma::Geometries::PersistenceLevel::Repository);
    }
    auto cube_mesh_id = PrimitiveId("cube_mesh", MeshPrimitive::TYPE_RTTI);
    auto cube_mesh = std::make_shared<QueryPrimitive>(cube_mesh_id)->dispatch();
    if (!cube_mesh)
    {
        MeshPrimitiveHelper mesh_helper(cube_mesh_id);
        cube_mesh = mesh_helper.geometryId(cube_geo_id).effect(EffectMaterialId("fx/default_textured_mesh_effect")).textureMap(EffectTextureMapDtoHelper().textureMapping(TextureId("image/one"), std::nullopt, "DiffuseMap")).visualTechnique("Default").renderListID(Renderer::RenderListID::Scene).asNative(cube_mesh_id.name() + ".mesh@DataPath").constitute(Enigma::Primitives::PersistenceLevel::Repository);
    }
    m_cubeId = SpatialId("cube_pawn", Pawn::TYPE_RTTI);
    m_cube = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(m_cubeId)->dispatch());
    if (!m_cube)
    {
        PawnHelper pawn_helper(m_cubeId);
        m_cube = pawn_helper.primitive(cube_mesh_id).localTransform(Matrix4::IDENTITY).topLevel(true).spatialFlags(SpatialShadowFlags::Spatial_ShadowCaster | SpatialShadowFlags::Spatial_ShadowReceiver).constitute(Enigma::SceneGraph::PersistenceLevel::Repository);
    }
    if ((m_sceneRoot) && (m_cube))
    {
        Matrix4 mx = Matrix4::MakeTranslateTransform(0.0f, 1.2f, 0.0f);
        m_sceneRoot->attachChild(m_cube, mx);
    }
    if (m_cube) m_cube->getPrimitive()->selectVisualTechnique("Default");
}
