#include "MeshPrimitiveTest.h"

#include <Geometries/TriangleList.h>

#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/PrimitiveRepositoryInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "Renderer/RendererEvents.h"
#include "CubeGeometryMaker.h"
#include "MeshPrimitiveMaker.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RenderableCommands.h"
#include "Renderer/RenderableEvents.h"
#include "Renderer/MeshPrimitive.h"
#include "GameEngine/Primitive.h"
#include "CameraMaker.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "Platforms/AndroidBridge.h"
#include "Gateways/DtoJsonGateway.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "Geometries/GeometryDataQueries.h"
#include "Frameworks/QueryDispatcher.h"

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
    m_onCameraConstituted = std::make_shared<EventSubscriber>([=](auto e) { this->onCameraConstituted(e); });
    EventPublisher::subscribe(typeid(CameraConstituted), m_onCameraConstituted);
    m_onRenderablePrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) {this->onRenderablePrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onBuildRenderablePrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) {this->onBuildRenderablePrimitiveFailed(e); });
    EventPublisher::subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) {this->onRendererCreated(e); });
    EventPublisher::subscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) {this->onRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>(std::make_shared<JsonFileDtoDeserializer>());
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt", std::make_shared<DtoJsonGateway>()));
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt", std::make_shared<DtoJsonGateway>()));
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(std::make_shared<JsonFileDtoDeserializer>(), std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt", std::make_shared<DtoJsonGateway>()));

    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, scene_graph_policy });

    makeCamera();

    makeCube();
}

void MeshPrimitiveTest::shutdownEngine()
{
    m_mesh = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;

    EventPublisher::unsubscribe(typeid(CameraConstituted), m_onCameraConstituted);
    m_onCameraConstituted = nullptr;
    EventPublisher::unsubscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onRenderablePrimitiveBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);
    m_onBuildRenderablePrimitiveFailed = nullptr;
    EventPublisher::unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void MeshPrimitiveTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if ((m_renderer) && (m_mesh))
    {
        m_mesh->insertToRendererWithTransformUpdating(m_renderer, Matrix4::IDENTITY, RenderLightingState{});
    }
}

void MeshPrimitiveTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
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
        CameraMaker::makeCamera(m_cameraId);
    }
}

void MeshPrimitiveTest::makeCube()
{
    m_cubeId = GeometryId("test_geometry");
    auto query = std::make_shared<QueryGeometryData>(m_cubeId);
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        MeshPrimitiveMaker::makeCubeMeshPrimitive("test_mesh", m_cubeId);
    }
    else
    {
        CubeGeometryMaker::makeCube(m_cubeId);
    }
}
void MeshPrimitiveTest::onCameraConstituted(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::CameraConstituted>(e);
    if (!ev) return;
    if (ev->id() != m_cameraId) return;
    m_camera = ev->camera();
    if ((m_camera) && (m_renderer)) m_renderer->SetAssociatedCamera(m_camera);
}

void MeshPrimitiveTest::onRenderablePrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(ev->getPrimitive());
    if (!m_isPrefabBuilt)
    {
        //MeshPrimitiveMaker::SaveMeshPrimitiveDto(mesh, "test_mesh.mesh@DataPath");
        //auto policy = MeshPrimitiveMaker::LoadMeshPrimitivePolicy("test_mesh.mesh@DataPath");
        //CommandBus::post(std::make_shared<Enigma::Renderer::BuildRenderablePrimitive>(policy));
        m_isPrefabBuilt = true;
    }
    else
    {
        m_mesh = mesh;
    }
}

void MeshPrimitiveTest::onBuildRenderablePrimitiveFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("renderable primitive %s build failed : %s\n", ev->getName().c_str(), ev->errorCode().message().c_str());
}

void MeshPrimitiveTest::onRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void MeshPrimitiveTest::onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}
