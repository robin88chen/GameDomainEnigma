#include "FindQuadVolumeTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Controllers/GraphicMain.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Animators/AnimatorCommands.h"
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

FindQuadVolumeTest::FindQuadVolumeTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
    m_culler = nullptr;
}

FindQuadVolumeTest::~FindQuadVolumeTest()
{
}

void FindQuadVolumeTest::initializeMountPaths()
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

void FindQuadVolumeTest::installEngine()
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
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy });

    m_sceneGraph = std::make_unique<NodalSceneGraph>(m_graphicMain->getSystemServiceAs<SceneGraphRepository>());
    makeCamera();
    makeModel();
}

void FindQuadVolumeTest::shutdownEngine()
{
    m_pawn = nullptr;
    m_sceneGraph->destroyRoot();
    m_sceneGraph = nullptr;
    //m_sceneRoot = nullptr;
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

void FindQuadVolumeTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    prepareRenderScene();
}

void FindQuadVolumeTest::makeCamera()
{
    /*m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);
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
    }*/
}

void FindQuadVolumeTest::makeModel()
{
    /*std::vector<std::string> meshNodeNames = { "bone_root", "bone_one" };
    auto cubeId = GeometryId("test_geometry");
    auto meshId = PrimitiveId("test_mesh", MeshPrimitive::TYPE_RTTI);
    auto modelId = PrimitiveId("test_model", ModelPrimitive::TYPE_RTTI);
    auto animationId = AnimationAssetId("test_animation");
    auto animatorId = AnimatorId("test_animator", ModelPrimitiveAnimator::TYPE_RTTI);
    auto pawn_id = SpatialId("pawn", Pawn::TYPE_RTTI);
    auto stillpawn_id = SpatialId("still_pawn", Pawn::TYPE_RTTI);
    auto animation = SkinAnimationMaker::makeSkinMeshAnimationAsset(animationId, meshNodeNames);
    auto animator = SkinAnimationMaker::makeModelAnimator(animatorId, animationId, modelId.origin(), meshId, meshNodeNames);
    auto cube = CubeGeometryMaker::makeCube(cubeId);
    auto mesh = SkinMeshModelMaker::makeCubeMeshPrimitive(meshId, cubeId);
    m_model = SkinMeshModelMaker::makeModelPrimitive(modelId, meshId, animatorId, meshNodeNames);
    m_rootId = SpatialId("root", Node::TYPE_RTTI);
    m_sceneGraph->createRoot(m_rootId);
    //m_sceneRoot = Node::queryNode(m_rootId);
    //if (!m_sceneRoot)
    if (!m_sceneGraph->root())
    {
        auto scene_dto = SceneGraphMaker::makeSceneGraph(m_rootId, modelId, pawn_id, stillpawn_id);
        std::make_shared<RequestSpatialConstitution>(m_rootId, scene_dto, PersistenceLevel::Store)->dispatch();
        m_sceneGraph->createRoot(m_rootId);
    }
    auto pawn = Pawn::queryPawn(pawn_id);
    if (!pawn) return;
    animatorId = pawn->getPrimitive()->animatorId();
    if (const auto animator = std::dynamic_pointer_cast<ModelPrimitiveAnimator>(Animator::queryAnimator(animatorId)))
    {
        animator->playAnimation(Enigma::Renderables::AnimationClip{ 0.0f, 2.5f, Enigma::Renderables::AnimationClip::WarpMode::Loop, 0 });
    }
    CommandBus::post(std::make_shared<AddListeningAnimator>(animatorId));*/
}

void FindQuadVolumeTest::prepareRenderScene()
{
    if ((m_sceneGraph) && (m_culler))
    {
        m_culler->ComputeVisibleSet(m_sceneGraph->root());
    }
    if ((m_renderer) && (m_culler))
    {
        m_renderer->prepareScene(m_culler->getVisibleSet());
    }
}

void FindQuadVolumeTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->beginScene();
    m_renderer->clearRenderTarget();
    m_renderer->drawScene();
    m_renderer->endScene();
    m_renderer->flip();
}

void FindQuadVolumeTest::onRendererCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->renderer());
    m_renderer->setAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

void FindQuadVolumeTest::onRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->renderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}
