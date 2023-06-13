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
#include "Frameworks/EventPublisher.h"
#include "GameCommon/GameSceneEvents.h"
#include "Frameworks/CommandBus.h"
#include "GameCommon/GameLightCommands.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "GameEngine/EffectDtoHelper.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"

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
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphRootCreated(e); });
    EventPublisher::Subscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPawnPrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

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
    auto primary_render_config = std::make_unique<SceneRendererServiceConfiguration>();
    auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, std::move(primary_render_config));
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
    m_sceneRoot = nullptr;
    m_floor = nullptr;
    m_cube = nullptr;

    EventPublisher::Unsubscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

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

void ShadowMapTest::OnSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
    CommandBus::Post(std::make_shared<CreateAmbientLight>(ev->GetSceneRoot(), "amb_lit", Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f)));
    CommandBus::Post(std::make_shared<CreateSunLight>(ev->GetSceneRoot(), "sun_lit", Enigma::MathLib::Vector3(-1.0f, -1.0f, -1.0f), Enigma::MathLib::ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
    auto mx = Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::Post(std::make_shared<CreatePointLight>(ev->GetSceneRoot(), mx, "point_lit", Vector3(2.0f, 2.0f, 2.0f), ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f), 3.50f));

    CreateFloorReceiver();
    CreateCubePawn();
}

void ShadowMapTest::OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    if (ev->GetSceneGraphId() == "floor_receiver")
    {
        m_floor = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
        if (m_sceneRoot) m_sceneRoot->AttachChild(m_floor, Matrix4::IDENTITY);
    }
    else if (ev->GetSceneGraphId() == "cube_pawn")
    {
        m_cube = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
        Matrix4 mx = Matrix4::MakeTranslateTransform(0.0f, 1.2f, 0.0f);
        if (m_sceneRoot) m_sceneRoot->AttachChild(m_cube, mx);
    }
}

void ShadowMapTest::OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetPawn() == m_floor)
    {
        ev->GetPawn()->GetPrimitive()->SelectVisualTechnique("ShadowMapReceiver");
    }
    else
    {
        ev->GetPawn()->GetPrimitive()->SelectVisualTechnique("Default");
    }
}

void ShadowMapTest::CreateFloorReceiver() const
{
    PawnDtoHelper pawn_dto("floor_receiver");
    MeshPrimitiveDto mesh_dto;
    SquareQuadDtoHelper floor_dto("floor");
    floor_dto.XZQuad(Vector3(-5.0f, 0.0f, -5.0f), Vector3(5.0f, 0.0f, 5.0f)).Normal().TextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper mat_dto("default_textured_mesh_effect");
    mat_dto.FilenameAtPath("fx/default_textured_mesh_effect.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.TextureMapping("image/du011.png", "APK_PATH", "du011", std::nullopt, "DiffuseMap");
    mesh_dto.Name() = "floor_mesh";
    mesh_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    mesh_dto.GeometryName() = "floor";
    mesh_dto.TheGeometry() = floor_dto.ToGenericDto();

    pawn_dto.MeshPrimitive(mesh_dto).LocalTransform(Matrix4::IDENTITY).TopLevel(true).SpatialFlags(Spatial::Spatial_BelongToParent | Spatial::Spatial_ShadowReceiver);
    auto dtos = { pawn_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>("floor_receiver", dtos));
}

void ShadowMapTest::CreateCubePawn() const
{
    PawnDtoHelper pawn_dto("cube_pawn");
    MeshPrimitiveDto mesh_dto;
    CubeDtoHelper cube_dto("cube");
    cube_dto.FacedCube(Vector3::ZERO, Vector3(1.0f, 1.0f, 1.0f)).FacedNormal().FacedTextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper mat_dto("default_textured_mesh_effect");
    mat_dto.FilenameAtPath("fx/default_textured_mesh_effect.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.TextureMapping("image/one.png", "APK_PATH", "one", std::nullopt, "DiffuseMap");
    mesh_dto.Name() = "cube_mesh";
    mesh_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    mesh_dto.GeometryName() = "cube";
    mesh_dto.TheGeometry() = cube_dto.ToGenericDto();

    pawn_dto.MeshPrimitive(mesh_dto).LocalTransform(Matrix4::IDENTITY).TopLevel(true).SpatialFlags(Spatial::Spatial_BelongToParent | Spatial::Spatial_ShadowCaster | Spatial::Spatial_ShadowReceiver);
    auto dtos = { pawn_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>("cube_pawn", dtos));
}
