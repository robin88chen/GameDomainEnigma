#include "AnimatedModelTest.h"
#include "Animators/AnimationAssetEvents.h"
#include "Animators/AnimatorCommands.h"
#include "Animators/AnimationAssetCommands.h"
#include "Animators/AnimatorEvents.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "CameraMaker.h"
#include "CubeGeometryMaker.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "FileSystem/AndroidMountPath.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Geometries/GeometryData.h"
#include "Geometries/GeometryDataEvents.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Geometries/GeometryCommands.h"
#include "ModelAnimatorMaker.h"
#include "ModelPrimitiveMaker.h"
#include "Platforms/AndroidBridge.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Primitives/PrimitiveCommands.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "Renderer/RendererEvents.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "SceneGraph/CameraFrustumCommands.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"

using namespace Enigma::Controllers;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Geometries;
using namespace Enigma::FileStorage;
using namespace Enigma::Primitives;
using namespace Enigma::SceneGraph;
using namespace Enigma::Gateways;
using namespace Enigma::Animators;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

AnimatedModelTest::AnimatedModelTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
}

AnimatedModelTest::~AnimatedModelTest()
{

}

void AnimatedModelTest::initializeMountPaths()
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

void AnimatedModelTest::installEngine()
{
    m_onCameraConstituted = std::make_shared<EventSubscriber>([=](auto e) { this->onCameraConstituted(e); });
    EventPublisher::subscribe(typeid(CameraConstituted), m_onCameraConstituted);

    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) {this->onRendererCreated(e); });
    EventPublisher::subscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) {this->onRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazied_scene.db.txt@DataPath", "lazy_", std::make_shared<DtoJsonGateway>()));
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>()));
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy });

    m_meshNodeNames = { "test_model_node_0", "test_model_node_1", "test_model_node_2", "test_model_node_3" };
    m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);
    m_cubeId = GeometryId("test_geometry");
    m_meshId = PrimitiveId("test_mesh", MeshPrimitive::TYPE_RTTI);
    m_modelId = PrimitiveId("test_model", ModelPrimitive::TYPE_RTTI);
    m_animationId = AnimationAssetId("test_animation");
    m_animatorId = AnimatorId("test_animator", ModelPrimitiveAnimator::TYPE_RTTI);
    makeCamera();
    makeModel();
}

void AnimatedModelTest::shutdownEngine()
{
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;

    EventPublisher::unsubscribe(typeid(CameraConstituted), m_onCameraConstituted);
    m_onCameraConstituted = nullptr;

    EventPublisher::unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void AnimatedModelTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if ((m_renderer) && (m_model))
    {
        m_model->insertToRendererWithTransformUpdating(m_renderer, Matrix4::IDENTITY, RenderLightingState{});
    }
}

void AnimatedModelTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->beginScene();
    m_renderer->clearRenderTarget();
    m_renderer->drawScene();
    m_renderer->endScene();
    m_renderer->flip();
}

void AnimatedModelTest::makeCamera()
{
    if (const auto camera = Camera::queryCamera(m_cameraId))
    {
        m_camera = camera;
        if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
    }
    else
    {
        CameraMaker::makeCamera(m_cameraId);
    }
}

void AnimatedModelTest::makeModel()
{
    auto animation = ModelAnimatorMaker::makeModelAnimationAsset(m_animationId, m_meshNodeNames[0]);
    if (animation) std::make_shared<PutAnimationAsset>(m_animationId, animation)->execute();
    auto animator = ModelAnimatorMaker::makeModelAnimator(m_animatorId, m_animationId, m_modelId.origin());
    if (animator) std::make_shared<PutAnimator>(m_animatorId, animator)->execute();
    auto cube = CubeGeometryMaker::makeCube(m_cubeId);
    if (cube) std::make_shared<PutGeometry>(m_cubeId, cube)->execute();
    auto mesh = ModelPrimitiveMaker::makeCubeMeshPrimitive(m_meshId, m_cubeId);
    if (mesh) std::make_shared<PutPrimitive>(m_meshId, mesh)->execute();
    m_model = ModelPrimitiveMaker::makeModelPrimitive(m_modelId.origin(), m_meshId, m_animatorId, m_meshNodeNames);
    if (m_model) std::make_shared<PutPrimitive>(m_modelId.origin(), m_model)->execute();
    m_animatorId = m_model->animatorId();
    if (const auto animator = std::dynamic_pointer_cast<ModelPrimitiveAnimator>(Animator::queryAnimator(m_animatorId)))
    {
        animator->playAnimation(Enigma::Renderables::AnimationClip{ 0.0f, 4.0f, Enigma::Renderables::AnimationClip::WarpMode::Loop, 0 });
    }
    CommandBus::enqueue(std::make_shared<AddListeningAnimator>(m_animatorId));
    m_model->updateWorldTransform(Matrix4::IDENTITY);
}

void AnimatedModelTest::onCameraConstituted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::CameraConstituted>(e);
    if (!ev) return;
    if (ev->id() != m_cameraId) return;
    if (ev->isPersisted()) return;
    m_camera = ev->camera();
    if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
    CommandBus::enqueue(std::make_shared<PutCamera>(m_cameraId, m_camera));
}

void AnimatedModelTest::onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->renderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

void AnimatedModelTest::onRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->renderer());
    m_renderer->setAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}
