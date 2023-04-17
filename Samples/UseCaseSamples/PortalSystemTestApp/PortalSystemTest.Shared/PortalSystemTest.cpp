#include "PortalSystemTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Renderer/RendererEvents.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/Culler.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "CameraMaker.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraphMaker.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::SceneGraph;
using namespace Enigma::Controllers;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::Platforms;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

PortalSystemTest::PortalSystemTest(const std::string app_name) : AppDelegate(app_name)
{
    m_culler = nullptr;
}

PortalSystemTest::~PortalSystemTest()
{
}

void PortalSystemTest::InitializeMountPaths()
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

void PortalSystemTest::InstallEngine()
{
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRendererCreated(e); });
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    m_onBuildPawnPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPawnPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    EventPublisher::Subscribe(typeid(BuildPawnPrimitiveFailed), m_onBuildPawnPrimitiveFailed);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<Enigma::Engine::DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<Enigma::Engine::EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<Enigma::Renderer::RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<Enigma::Animators::AnimatorInstallingPolicy>();
    auto renderer_policy = std::make_shared<Enigma::Renderer::DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto scene_graph_policy = std::make_shared<Enigma::SceneGraph::SceneGraphInstallingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>());
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, renderer_policy, scene_graph_policy });

    m_camera = CameraMaker::MakeCamera();
    m_culler = menew Culler(m_camera);

    PrimitiveMeshMaker::MakeSavedFloorGeometry("floor");
    PrimitiveMeshMaker::MakeSavedDoorGeometry("door");
    PrimitiveMeshMaker::MakeSavedBoardGeometry("board");
    //auto dtos = SceneGraphMaker::MakeFloorDtos("floor_node");
    //SceneGraphMaker::MakeSavedPortalSceneGraph("portal_scene");
    auto dtos = SceneGraphMaker::MakePortalSceneGraph("portal_scene");
    CommandBus::Post(std::make_shared<BuildSceneGraph>("test_portal", dtos));
}

void PortalSystemTest::ShutdownEngine()
{
    m_pawn = nullptr;
    m_sceneRoot = nullptr;
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;
    SAFE_DELETE(m_culler);
    EventPublisher::Unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    EventPublisher::Unsubscribe(typeid(BuildPawnPrimitiveFailed), m_onBuildPawnPrimitiveFailed);
    m_onRendererCreated = nullptr;
    m_onRenderTargetCreated = nullptr;
    m_onSceneGraphBuilt = nullptr;
    m_onPawnPrimitiveBuilt = nullptr;
    m_onBuildPawnPrimitiveFailed = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void PortalSystemTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();

    PrepareRenderScene();
}

void PortalSystemTest::RenderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
}

void PortalSystemTest::PrepareRenderScene()
{
    if (m_sceneRoot)
    {
        m_culler->ComputeVisibleSet(m_sceneRoot);
    }
    if ((m_renderer) && (m_sceneRoot) && (m_culler->GetVisibleSet().GetCount() != 0))
    {
        m_renderer->PrepareScene(m_culler->GetVisibleSet());
    }
}

void PortalSystemTest::OnRendererCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void PortalSystemTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void PortalSystemTest::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    m_sceneRoot = std::dynamic_pointer_cast<Node, Spatial>(top_spatials[0]);
    if (!m_sceneRoot) return;
}

void PortalSystemTest::OnBuildPawnPrimitiveFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildPawnPrimitiveFailed, IEvent>(e);
    if (!ev) return;
    Debug::ErrorPrintf("build pawn primitive failed %s\n", ev->GetErrorCode().message().c_str());
}
