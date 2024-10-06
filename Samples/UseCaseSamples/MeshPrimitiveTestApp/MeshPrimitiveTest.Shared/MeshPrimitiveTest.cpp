﻿#include "MeshPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Renderer/RendererEvents.h"
#include "CubeGeometryMaker.h"
#include "MeshPrimitiveMaker.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Renderables/MeshPrimitive.h"
#include "Primitives/Primitive.h"
#include "CameraMaker.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Platforms/AndroidBridge.h"
#include "Gateways/DtoJsonGateway.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include "SceneGraph/CameraFrustumCommands.h"
#include "Primitives/PrimitiveEvents.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "Geometries/GeometryCommands.h"
#include "Primitives/PrimitiveCommands.h"

using namespace Enigma::FileSystem;
using namespace Enigma::Controllers;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Geometries;
using namespace Enigma::FileStorage;
using namespace Enigma::Gateways;
using namespace Enigma::Primitives;
using namespace Enigma::Renderables;
using namespace Enigma::Animators;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

MeshPrimitiveTest::MeshPrimitiveTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
}

MeshPrimitiveTest::~MeshPrimitiveTest()
{

}

void MeshPrimitiveTest::initializeMountPaths()
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

void MeshPrimitiveTest::installEngine()
{
    //m_onCameraConstituted = std::make_shared<EventSubscriber>([=](auto e) { this->onCameraConstituted(e); });
    //EventPublisher::subscribe(typeid(CameraConstituted), m_onCameraConstituted);

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
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy, animator_policy });

    makeCamera();

    makeMesh();
}

void MeshPrimitiveTest::shutdownEngine()
{
    m_mesh = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;

    //EventPublisher::unsubscribe(typeid(CameraConstituted), m_onCameraConstituted);
    //m_onCameraConstituted = nullptr;

    EventPublisher::unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void MeshPrimitiveTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if ((m_renderer) && (m_mesh) && (m_mesh->lazyStatus().isReady()))
    {
        m_mesh->insertToRendererWithTransformUpdating(m_renderer, Matrix4::IDENTITY, RenderLightingState{});
    }
}

void MeshPrimitiveTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->beginScene();
    m_renderer->clearRenderTarget();
    m_renderer->drawScene();
    m_renderer->endScene();
    m_renderer->flip();
}

void MeshPrimitiveTest::makeCamera()
{
    m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);
    auto query = std::make_shared<QueryCamera>(m_cameraId);
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        m_camera = query->getResult();
    }
    else
    {
        m_camera = CameraMaker::makeCamera(m_cameraId);
    }
    if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
}

void MeshPrimitiveTest::makeMesh()
{
    m_cubeId = GeometryId("test_geometry");
    auto cube = CubeGeometryMaker::makeCube(m_cubeId);
    if (cube) CommandBus::enqueue(std::make_shared<PutGeometry>(m_cubeId, cube));
    m_meshId = PrimitiveId("test_mesh", MeshPrimitive::TYPE_RTTI);
    m_mesh = MeshPrimitiveMaker::makeCubeMeshPrimitive(m_meshId, m_cubeId);
    if (m_mesh) CommandBus::enqueue(std::make_shared<PutPrimitive>(m_meshId, m_mesh));
}

/*void MeshPrimitiveTest::onCameraConstituted(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::CameraConstituted>(e);
    if (!ev) return;
    if (ev->id() != m_cameraId) return;
    if (ev->isPersisted()) return;
    m_camera = ev->camera();
    if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
    CommandBus::enqueue(std::make_shared<PutCamera>(m_cameraId, m_camera));
}*/

void MeshPrimitiveTest::onRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->renderer());
    if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

void MeshPrimitiveTest::onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->renderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}
