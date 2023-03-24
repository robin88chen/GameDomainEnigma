#include "SceneGraphPawnTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Controllers/GraphicMain.h"
#include "Controllers/InstallingPolicies.h"
#include "Frameworks/CommandBus.h"
#include "CubeGeometryMaker.h"
#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameEngine/Primitive.h"
#include "CameraMaker.h"
#include "Animators/AnimatorCommands.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "SceneGraph/Culler.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Gateways/DtoJsonGateway.h"

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Renderer;
using namespace Enigma::Controllers;
using namespace Enigma::SceneGraph;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::Animators;

SceneGraphPawnTest::SceneGraphPawnTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
}

SceneGraphPawnTest::~SceneGraphPawnTest()
{
}

void SceneGraphPawnTest::InitializeMountPaths()
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

void SceneGraphPawnTest::InstallEngine()
{
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRendererCreated(e); });
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto scene_graph_policy = std::make_shared<SceneGraphBuildingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>(), std::make_shared<JsonFileEffectProfileDeserializer>());
    m_graphicMain->InstallRenderEngine({ creating_policy, renderer_policy, scene_graph_policy });
    CubeGeometryMaker::MakeSavedCube("test_geometry");
    auto dtos = SceneGraphMaker::MakeSceneGraphDtos();
    std::string json = DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("scene_graph.ctt@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);

    CommandBus::Post(std::make_shared<BuildSceneGraph>("test_scene", dtos));

    m_camera = CameraMaker::MakeCamera();
    m_culler = menew Culler(m_camera);
}

void SceneGraphPawnTest::ShutdownEngine()
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

void SceneGraphPawnTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    //RetrieveDtoCreatedModel();
    //InsertDtoCreatedModelToRenderer();

    RetrieveDtoCreatedModel();
    PrepareRenderScene();
}

void SceneGraphPawnTest::RetrieveDtoCreatedModel()
{
    if ((m_pawn) && (!m_model))
    {
        auto prim = m_pawn->GetPrimitive();
        if (prim)
        {
            m_model = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(prim);
            if (auto ani = m_model->GetAnimator())
            {
                CommandBus::Post(std::make_shared<AddListeningAnimator>(ani));
                if (auto model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(ani))
                {
                    model_ani->PlayAnimation(AnimationClip{ 0.0f, 2.0f, AnimationClip::WarpMode::Loop, 0 });
                }
            }
        }
    }
}

void SceneGraphPawnTest::InsertDtoCreatedModelToRenderer()
{
    if ((m_renderer) && (m_model))
    {
        m_model->InsertToRendererWithTransformUpdating(m_renderer, Enigma::MathLib::Matrix4::IDENTITY, RenderLightingState{});
    }
}

void SceneGraphPawnTest::PrepareRenderScene()
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

void SceneGraphPawnTest::RenderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
}

void SceneGraphPawnTest::OnRendererCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void SceneGraphPawnTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void SceneGraphPawnTest::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    m_sceneRoot = std::dynamic_pointer_cast<Node, Spatial>(top_spatials[0]);
    if (!m_sceneRoot) return;
    for (auto child : m_sceneRoot->GetChildList())
    {
        if (child->GetSpatialName() == "child2")
        {
            m_pawn = std::dynamic_pointer_cast<Pawn, Spatial>(std::dynamic_pointer_cast<Node, Spatial>(child)->GetChildList().front());
        }
    }
}
