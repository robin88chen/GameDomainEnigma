#include "PortalManagementTest.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraphMaker.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "Gateways/DtoJsonGateway.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "Platforms/MemoryMacro.h"
#include "SceneGraph/Culler.h"
#include "Controllers/ControllerEvents.h"
#include "Renderables/MeshPrimitive.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/Portal.h"
#include "Frameworks/CommandBus.h"
#include "GameCommon/GameSceneCommands.h"

using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::FileStorage;
using namespace Enigma::Gateways;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::Platforms;
using namespace Enigma::Animators;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::InputHandlers;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Controllers;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

PortalManagementTest::PortalManagementTest(const std::string app_name) : AppDelegate(app_name)
{
    m_culler = nullptr;
}

PortalManagementTest::~PortalManagementTest()
{
}

void PortalManagementTest::initializeMountPaths()
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

void PortalManagementTest::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);

    assert(m_graphicMain);

    m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    //auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    m_geometryDataFileStoreMapper = std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(m_geometryDataFileStoreMapper);
    m_primitiveFileStoreMapper = std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(m_primitiveFileStoreMapper);
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    m_sceneGraphFileStoreMapper = std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    auto input_handler_policy = std::make_shared<InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(m_cameraId,
        CameraAssembler(m_cameraId).eyePosition(Vector3(-10.0f, 10.0f, -10.0f)).lookAt(Vector3(1.0f, -1.0f, 1.0f)).upDirection(Vector3::UNIT_Y).frustum(Frustum::ProjectionType::Perspective).frustumFov(Math::PI / 4.0f).frustumFrontBackZ(0.1f, 100.0f).frustumNearPlaneDimension(40.0f, 30.0f).asNative(m_cameraId.name() + ".cam@DataPath").toCameraDto().toGenericDto());
    auto primary_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, primary_render_config);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>();

    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy, input_handler_policy, game_camera_policy, scene_renderer_policy, game_scene_policy });

    m_inputHandler = input_handler_policy->GetInputHandler();
#if TARGET_PLATFORM == PLATFORM_ANDROID
    ApplicationBridge::InitInputHandler(input_handler_policy->GetInputHandler());
#endif
    m_sceneRendererService = m_graphicMain->getSystemServiceAs<SceneRendererService>();
}

void PortalManagementTest::shutdownEngine()
{
    m_pawn = nullptr;
    m_sceneRoot = nullptr;
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;
    SAFE_DELETE(m_culler);

    m_graphicMain->shutdownRenderEngine();

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
}

void PortalManagementTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    prepareRenderScene();
}

void PortalManagementTest::renderFrame()
{
    if (!m_sceneRendererService.expired())
    {
        m_sceneRendererService.lock()->renderGameScene();
        m_sceneRendererService.lock()->flip();
    }
}

void PortalManagementTest::makeSceneGraph()
{
    auto floor_id = GeometryId("floor");
    if (!m_geometryDataFileStoreMapper->hasGeometry(floor_id))
    {
        const auto floor_dto = PrimitiveMeshMaker::makeFloorGeometry(floor_id);
        m_geometryDataFileStoreMapper->putGeometry(floor_id, floor_dto);
    }
    auto door_id = GeometryId("door");
    if (!m_geometryDataFileStoreMapper->hasGeometry(door_id))
    {
        const auto door_dto = PrimitiveMeshMaker::makeDoorGeometry(door_id);
        m_geometryDataFileStoreMapper->putGeometry(door_id, door_dto);
    }
    auto cube_id = GeometryId("cube");
    if (!m_geometryDataFileStoreMapper->hasGeometry(cube_id))
    {
        const auto cube_dto = PrimitiveMeshMaker::makeCubeGeometry(cube_id);
        m_geometryDataFileStoreMapper->putGeometry(cube_id, cube_dto);
    }
    auto floor_efx_id = EffectMaterialId("fx/basic_floor_mesh");
    auto du011_tex_id = TextureId("image/du011");
    auto earth_tex_id = TextureId("earth");
    auto hga001_tex_id = TextureId("image/hga0010101");
    auto floor_mesh_id = PrimitiveId("floor_mesh", MeshPrimitive::TYPE_RTTI);
    if (!m_primitiveFileStoreMapper->hasPrimitive(floor_mesh_id))
    {
        const auto floor_mesh_dto = PrimitiveMeshMaker::makeMeshPrimitive(floor_mesh_id, floor_id, floor_efx_id, du011_tex_id, "DiffuseMap");
        m_primitiveFileStoreMapper->putPrimitive(floor_mesh_id, floor_mesh_dto);
    }
    auto door_mesh_id = PrimitiveId("door_mesh", MeshPrimitive::TYPE_RTTI);
    if (!m_primitiveFileStoreMapper->hasPrimitive(door_mesh_id))
    {
        const auto door_mesh_dto = PrimitiveMeshMaker::makeMeshPrimitive(door_mesh_id, door_id, floor_efx_id, earth_tex_id, "DiffuseMap");
        m_primitiveFileStoreMapper->putPrimitive(door_mesh_id, door_mesh_dto);
    }
    auto cube_mesh_id = PrimitiveId("cube_mesh", MeshPrimitive::TYPE_RTTI);
    if (!m_primitiveFileStoreMapper->hasPrimitive(cube_mesh_id))
    {
        const auto cube_mesh_dto = PrimitiveMeshMaker::makeMeshPrimitive(cube_mesh_id, cube_id, floor_efx_id, hga001_tex_id, "DiffuseMap");
        m_primitiveFileStoreMapper->putPrimitive(cube_mesh_id, cube_mesh_dto);
    }

    auto floor_pawn_id = SpatialId("floor_pawn", Pawn::TYPE_RTTI);
    if (!m_sceneGraphFileStoreMapper->hasSpatial(floor_pawn_id))
    {
        const auto floor_pawn_dto = SceneGraphMaker::makePawm(floor_pawn_id, floor_mesh_id, PrimitiveMeshMaker::getFloorBound());
        m_sceneGraphFileStoreMapper->putSpatial(floor_pawn_id, floor_pawn_dto);
    }
    auto door_pawn_id = SpatialId("door_pawn", Pawn::TYPE_RTTI);
    if (!m_sceneGraphFileStoreMapper->hasSpatial(door_pawn_id))
    {
        const auto door_pawn_dto = SceneGraphMaker::makePawm(door_pawn_id, door_mesh_id, PrimitiveMeshMaker::getDoorBound());
        m_sceneGraphFileStoreMapper->putSpatial(door_pawn_id, door_pawn_dto);
    }
    auto cube_pawn_id = SpatialId("cube_pawn", Pawn::TYPE_RTTI);
    if (!m_sceneGraphFileStoreMapper->hasSpatial(cube_pawn_id))
    {
        const auto cube_pawn_dto = SceneGraphMaker::makePawm(cube_pawn_id, cube_mesh_id, PrimitiveMeshMaker::getBoardBound());
        m_sceneGraphFileStoreMapper->putSpatial(cube_pawn_id, cube_pawn_dto);
    }

    m_rootId = SpatialId("root", Node::TYPE_RTTI);
    m_portalManagementId = SpatialId("portal_management", PortalManagementNode::TYPE_RTTI);
    auto outside_region_id = SpatialId("outside_region", PortalZoneNode::TYPE_RTTI);
    auto portal_id = SpatialId("portal", Portal::TYPE_RTTI);
    auto inside_zone_node_id = SpatialId("inside_zone_node", PortalZoneNode::TYPE_RTTI);
    PortalZoneNodeAssembler inside_zone_assembler(inside_zone_node_id);
    PortalZoneNodeAssembler outside_region_assembler(outside_region_id);
    if (!m_sceneGraphFileStoreMapper->hasLaziedContent(outside_region_id))
    {
        outside_region_assembler = SceneGraphMaker::makeOutsideRegion(outside_region_id, m_portalManagementId, portal_id, inside_zone_node_id, { floor_pawn_id, door_pawn_id });
        m_sceneGraphFileStoreMapper->putLaziedContent(outside_region_id, outside_region_assembler.toHydratedGenericDto());
    }
    if (!m_sceneGraphFileStoreMapper->hasLaziedContent(inside_zone_node_id))
    {
        inside_zone_assembler = SceneGraphMaker::makeInsideZoneNode(inside_zone_node_id, portal_id, { cube_pawn_id });
        m_sceneGraphFileStoreMapper->putLaziedContent(inside_zone_node_id, inside_zone_assembler.toHydratedGenericDto());
    }
    if (!m_sceneGraphFileStoreMapper->hasSpatial(m_portalManagementId))
    {
        auto portal_management_dto = SceneGraphMaker::makePortalManagementNode(m_portalManagementId, outside_region_assembler, inside_zone_assembler);
        m_sceneGraphFileStoreMapper->putSpatial(m_portalManagementId, portal_management_dto);
    }
    if (!m_sceneGraphFileStoreMapper->hasSpatial(m_rootId))
    {
        NodeAssembler root_assembler(m_rootId);
        m_sceneGraphFileStoreMapper->putSpatial(m_rootId, root_assembler.toGenericDto());
    }
    CommandBus::post(std::make_shared<CreateSceneRoot>(m_rootId, m_portalManagementId));
}

void PortalManagementTest::prepareRenderScene()
{
    if (!m_sceneRendererService.expired()) m_sceneRendererService.lock()->prepareGameScene();
}

void PortalManagementTest::onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e)
{
    if ((!e) || (e->typeInfo() != typeid(RenderEngineInstalled))) return;
    makeSceneGraph();
}
