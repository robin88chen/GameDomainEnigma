#include "CreateFittingNodeTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventPublisher.h"
#include "Controllers/GraphicMain.h"
#include "SceneGraph/SceneGraphEvents.h"
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
#include "SceneGraph/SceneGraphRepository.h"
#include "Controllers/ControllerEvents.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "WorldMap/QuadTreeRoot.h"
#include "WorldMap/QuadTreeVolume.h"
#include "WorldMap/QuadTreeRootDto.h"
#include "SceneGraphMaker.h"

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
using namespace Enigma::WorldMap;

CreateFittingNodeTest::CreateFittingNodeTest(const std::string app_name) : AppDelegate(app_name)
{
}

CreateFittingNodeTest::~CreateFittingNodeTest()
{
}

void CreateFittingNodeTest::initializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../../Media/";
        auto dataPath = path / "Data/";
        if (!std::filesystem::exists(dataPath)) std::filesystem::create_directory(dataPath);
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
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

void CreateFittingNodeTest::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    m_sceneGraphFileStoreMapper = std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", "lazied_", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy });
    m_sceneGraphRepository = m_graphicMain->getSystemServiceAs<SceneGraphRepository>();
}

void CreateFittingNodeTest::shutdownEngine()
{
    m_sceneGraphRepository = nullptr;
    m_sceneGraphFileStoreMapper = nullptr;
    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void CreateFittingNodeTest::frameUpdate()
{
    AppDelegate::frameUpdate();
}

void CreateFittingNodeTest::makeOneLevelQuad()
{
    assert(m_sceneGraphFileStoreMapper);
    m_rootQuadId = SpatialId("root_quad", VisibilityManagedNode::TYPE_RTTI);
    SceneGraphMaker::makeOneLevelQuadNode(m_rootQuadId, m_sceneGraphFileStoreMapper);
}

void CreateFittingNodeTest::testFittingNodeCreation()
{
    auto quad_tree_id = QuadTreeRootId("quad_tree_root");
    QuadTreeRootDto dto;
    dto.id() = quad_tree_id;
    dto.rootNodeId() = m_rootQuadId;
    auto quad_tree_root = std::make_shared<QuadTreeRoot>(quad_tree_id, dto.toGenericDto());
    BoundingVolume bv_at_origin = BoundingVolume(Box3::UNIT_BOX);
    auto fitted = quad_tree_root->findFittingNode(bv_at_origin);
    assert(fitted.has_value() && fitted.value() == m_rootQuadId);
    m_level1QuadId3 = SpatialId("root_quad_3", VisibilityManagedNode::TYPE_RTTI);
    Box3 box_at_x4z2 = Box3::UNIT_BOX;
    box_at_x4z2.Center().x() = 4.0f;
    box_at_x4z2.Center().z() = 2.0f;
    BoundingVolume bv_at_x4z2 = BoundingVolume(box_at_x4z2);
    fitted = quad_tree_root->findFittingNode(bv_at_x4z2);
    assert(fitted.has_value());
    if (fitted.value() == m_rootQuadId) // at first time, the fitting node is the root node
    {
        quad_tree_root->createFittingNodes(m_sceneGraphRepository, bv_at_x4z2, 4);
        fitted = quad_tree_root->findFittingNode(bv_at_x4z2);
    }
    assert(fitted.has_value() && fitted.value() == m_level1QuadId3);
    m_level4QuadId2 = SpatialId("root_quad_0_0_0_2", VisibilityManagedNode::TYPE_RTTI);
    Box3 box_at_xn_zn = Box3::UNIT_BOX;
    box_at_xn_zn.Center().x() = -6.3f;
    box_at_xn_zn.Center().z() = -7.3f;
    box_at_xn_zn.Extent(0) = 0.1f;
    box_at_xn_zn.Extent(2) = 0.1f;
    BoundingVolume bv_at_xn_zn = BoundingVolume(box_at_xn_zn);
    fitted = quad_tree_root->findFittingNode(bv_at_xn_zn);
    assert(fitted.has_value());
    if (fitted.value() == m_rootQuadId) // at first time, the fitting node is the root node
    {
        quad_tree_root->createFittingNodes(m_sceneGraphRepository, bv_at_xn_zn, 4);
        fitted = quad_tree_root->findFittingNode(bv_at_xn_zn);
    }
    assert(fitted.has_value() && fitted.value() == m_level4QuadId2);
    m_level3QuadId0 = SpatialId("root_quad_0_0_0", VisibilityManagedNode::TYPE_RTTI);
    Box3 box_at_xn7_zn7 = Box3::UNIT_BOX;
    box_at_xn7_zn7.Center().x() = -7.1f;
    box_at_xn7_zn7.Center().z() = -7.1f;
    box_at_xn7_zn7.Extent(0) = 0.6f;
    box_at_xn7_zn7.Extent(2) = 0.6f;
    BoundingVolume bv_at_xn7_zn7 = BoundingVolume(box_at_xn7_zn7);
    fitted = quad_tree_root->findFittingNode(bv_at_xn7_zn7);
    assert(fitted.has_value() && fitted.value() == m_level3QuadId0);
}

void CreateFittingNodeTest::renderFrame()
{
}

void CreateFittingNodeTest::onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderEngineInstalled>(e);
    if (!ev) return;
    makeOneLevelQuad();
    testFittingNodeCreation();
}
