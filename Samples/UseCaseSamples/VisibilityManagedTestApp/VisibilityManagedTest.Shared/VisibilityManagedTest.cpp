#include "VisibilityManagedTest.h"
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
#include "Controllers/InstallingPolicies.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "CameraMaker.h"
#include "SceneGraph/Culler.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "CubeGeometryMaker.h"
#include "SceneGraphMaker.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::SceneGraph;
using namespace Enigma::Controllers;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

VisibilityManagedTest::VisibilityManagedTest(const std::string app_name) : AppDelegate(app_name)
{
    m_culler = nullptr;
}

VisibilityManagedTest::~VisibilityManagedTest()
{

}

void VisibilityManagedTest::InitializeMountPaths()
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

void VisibilityManagedTest::InstallEngine()
{
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRendererCreated(e); });
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto renderer_policy = std::make_shared<InstallingDefaultRendererPolicy>(DefaultRendererName, PrimaryTargetName);
    auto scene_graph_policy = std::make_shared<SceneGraphBuildingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>(), std::make_shared<JsonFileEffectProfileDeserializer>());
    m_graphicMain->InstallRenderEngine({ creating_policy, renderer_policy, scene_graph_policy });

    CubeGeometryMaker::MakeSavedCube("test_geometry");
    SceneGraphMaker::MakeSavedVisibilityNode("visibility");
    auto dtos = SceneGraphMaker::MakeSceneGraphDtos("visibility");

    CommandBus::Post(std::make_shared<BuildSceneGraph>("test_scene", dtos));

    m_camera = CameraMaker::MakeCamera();
    m_culler = menew Culler(m_camera);
}

void VisibilityManagedTest::ShutdownEngine()
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
    m_onRendererCreated = nullptr;
    m_onRenderTargetCreated = nullptr;
    m_onSceneGraphBuilt = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void VisibilityManagedTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();

    PrepareRenderScene();
}

void VisibilityManagedTest::RenderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
}

void VisibilityManagedTest::PrepareRenderScene()
{
    if (m_sceneRoot)
    {
        m_culler->ComputeVisibleSet(m_sceneRoot);
    }
    if (m_renderer)
    {
        m_renderer->PrepareScene(m_culler->GetVisibleSet());
    }
}

void VisibilityManagedTest::OnRendererCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void VisibilityManagedTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void VisibilityManagedTest::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    m_sceneRoot = std::dynamic_pointer_cast<Node, Spatial>(top_spatials[0]);
    if (!m_sceneRoot) return;
}
