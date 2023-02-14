#include "SceneGraphPawnTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Controllers/GraphicMain.h"
#include "Controllers/InstallingPolicies.h"
#include "Frameworks/CommandBus.h"
#include "CubeGeometryMaker.h"
#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphCommands.h"

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Renderer;
using namespace Enigma::Controllers;
using namespace Enigma::SceneGraph;

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
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRendererCreated(e); });
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto renderer_policy = std::make_shared<InstallingDefaultRendererPolicy>(DefaultRendererName, PrimaryTargetName);
    auto scene_graph_policy = std::make_shared<SceneGraphBuildingPolicy>(nullptr, nullptr);
    m_graphicMain->InstallRenderEngine({ creating_policy, renderer_policy, scene_graph_policy });
    CubeGeometryMaker::MakeSavedCube("test_geometry");
    CommandBus::Post(std::make_shared<BuildSceneGraph>("test_scene", SceneGraphMaker::MakeSceneGraphDtos()));
}

void SceneGraphPawnTest::ShutdownEngine()
{
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;

    EventPublisher::Unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRendererCreated = nullptr;
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void SceneGraphPawnTest::FrameUpdate()
{

}

void SceneGraphPawnTest::RenderFrame()
{

}

void SceneGraphPawnTest::OnRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{

}

void SceneGraphPawnTest::OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{

}
