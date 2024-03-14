#include "PortalSystemTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Controllers/GraphicMain.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "CameraMaker.h"
#include "SceneGraph/Culler.h"
#include "Platforms/MemoryMacro.h"
#include "Gateways/DtoJsonGateway.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphPersistenceLevel.h"
#include "SceneGraph/LazyNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/Portal.h"
#include "SceneGraph/PortalManagementNode.h"

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

using namespace Enigma::Controllers;
using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Renderer;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::FileStorage;
using namespace Enigma::Gateways;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::SceneGraph;

PortalSystemTest::PortalSystemTest(const std::string app_name) : AppDelegate(app_name)
{
    m_culler = nullptr;
}

PortalSystemTest::~PortalSystemTest()
{
}

void PortalSystemTest::initializeMountPaths()
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

void PortalSystemTest::installEngine()
{
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onRendererCreated(e); });
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
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
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy });

    makeCamera();
    makeSceneGraph();
}

void PortalSystemTest::shutdownEngine()
{
    m_pawn = nullptr;
    m_sceneRoot = nullptr;
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;
    SAFE_DELETE(m_culler);
    EventPublisher::unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRendererCreated = nullptr;
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void PortalSystemTest::frameUpdate()
{
    AppDelegate::frameUpdate();

    prepareRenderScene();
}

void PortalSystemTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->beginScene();
    m_renderer->clearRenderTarget();
    m_renderer->drawScene();
    m_renderer->endScene();
    m_renderer->flip();
}

void PortalSystemTest::prepareRenderScene()
{
    if (m_sceneRoot)
    {
        m_culler->ComputeVisibleSet(m_sceneRoot);
    }
    if ((m_renderer) && (m_sceneRoot) && (m_culler->getVisibleSet().getCount() != 0))
    {
        m_renderer->prepareScene(m_culler->getVisibleSet());
    }
}
void PortalSystemTest::makeCamera()
{
    m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);
    if (const auto camera = Camera::queryCamera(m_cameraId))
    {
        m_camera = camera;
        if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
        m_culler = menew Culler(m_camera);
    }
    else
    {
        m_camera = CameraMaker::makeCamera(m_cameraId);
        m_culler = menew Culler(m_camera);
    }
}

void PortalSystemTest::makeSceneGraph()
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
    auto board_id = GeometryId("board");
    if (!m_geometryDataFileStoreMapper->hasGeometry(board_id))
    {
        const auto board_dto = PrimitiveMeshMaker::makeBoardGeometry(board_id);
        m_geometryDataFileStoreMapper->putGeometry(board_id, board_dto);
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
    auto board_mesh_id = PrimitiveId("board_mesh", MeshPrimitive::TYPE_RTTI);
    if (!m_primitiveFileStoreMapper->hasPrimitive(board_mesh_id))
    {
        const auto board_mesh_dto = PrimitiveMeshMaker::makeMeshPrimitive(board_mesh_id, board_id, floor_efx_id, hga001_tex_id, "DiffuseMap");
        m_primitiveFileStoreMapper->putPrimitive(board_mesh_id, board_mesh_dto);
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
    auto board_pawn_id = SpatialId("board_pawn", Pawn::TYPE_RTTI);
    if (!m_sceneGraphFileStoreMapper->hasSpatial(board_pawn_id))
    {
        const auto board_pawn_dto = SceneGraphMaker::makePawm(board_pawn_id, board_mesh_id, PrimitiveMeshMaker::getBoardBound());
        m_sceneGraphFileStoreMapper->putSpatial(board_pawn_id, board_pawn_dto);
    }

    m_rootId = SpatialId("root", PortalManagementNode::TYPE_RTTI);
    auto outside_region_id = SpatialId("outside_region", PortalZoneNode::TYPE_RTTI);
    auto portal_id = SpatialId("portal", Portal::TYPE_RTTI);
    auto inside_zone_node_id = SpatialId("inside_zone_node", PortalZoneNode::TYPE_RTTI);
    if (!m_sceneGraphFileStoreMapper->hasLaziedContent(outside_region_id))
    {
        const auto outside_region_dto = SceneGraphMaker::makeOutsideRegion(outside_region_id, m_rootId, portal_id, inside_zone_node_id, { floor_pawn_id, door_pawn_id });
        m_sceneGraphFileStoreMapper->putLaziedContent(outside_region_id, outside_region_dto);
    }
    if (!m_sceneGraphFileStoreMapper->hasLaziedContent(inside_zone_node_id))
    {
        const auto inside_zone_node_dto = SceneGraphMaker::makeInsideZoneNode(inside_zone_node_id, portal_id, { board_pawn_id });
        m_sceneGraphFileStoreMapper->putLaziedContent(inside_zone_node_id, inside_zone_node_dto);
    }
    m_sceneRoot = Node::queryNode(m_rootId);
    if (!m_sceneRoot)
    {
        auto root_dto = SceneGraphMaker::makeSceneGraph(m_rootId, outside_region_id, portal_id, inside_zone_node_id);
        m_sceneRoot = std::dynamic_pointer_cast<Node>(std::make_shared<RequestSpatialConstitution>(m_rootId, root_dto, PersistenceLevel::Store)->dispatch());
    }
}

void PortalSystemTest::onRendererCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->renderer());
    m_renderer->setAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

void PortalSystemTest::onRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->renderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}
