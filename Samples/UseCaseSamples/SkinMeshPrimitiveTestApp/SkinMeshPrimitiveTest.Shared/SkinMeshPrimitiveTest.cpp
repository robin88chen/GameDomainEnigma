#include "SkinMeshPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RenderablePrimitiveEvents.h"
#include "Renderer/RenderablePrimitiveCommands.h"
#include "Renderer/RendererEvents.h"
#include "Controllers/GraphicMain.h"
#include "Controllers/InstallingPolicies.h"
#include "Frameworks/CommandBus.h"
#include "CubeGeometryMaker.h"
#include "CameraMaker.h"
#include "SkinMeshModelMaker.h"
#include <string>

using namespace Enigma::Controllers;
using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Renderer;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

SkinMeshPrimitiveTest::SkinMeshPrimitiveTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
}

SkinMeshPrimitiveTest::~SkinMeshPrimitiveTest()
{

}

void SkinMeshPrimitiveTest::InitializeMountPaths()
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

void SkinMeshPrimitiveTest::InstallEngine()
{
    m_onRenderablePrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) {this->OnRenderablePrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onBuildRenderablePrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) {this->OnBuildRenderablePrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRendererCreated(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));

    CubeGeometryMaker::MakeSavedCube("test_geometry");
    auto prim_policy = SkinMeshModelMaker::MakeModelPrimitivePolicy("test_model", "test_geometry");
    CommandBus::Post(std::make_shared<Enigma::Renderer::BuildRenderablePrimitive>(prim_policy));

    m_camera = CameraMaker::MakeCamera();

}

void SkinMeshPrimitiveTest::ShutdownEngine()
{

}

void SkinMeshPrimitiveTest::FrameUpdate()
{

}

void SkinMeshPrimitiveTest::RenderFrame()
{

}

void SkinMeshPrimitiveTest::OnRendererCreated(const IEventPtr& e)
{

}

void SkinMeshPrimitiveTest::OnRenderTargetCreated(const IEventPtr& e)
{

}

void SkinMeshPrimitiveTest::OnRenderablePrimitiveBuilt(const IEventPtr& e)
{

}

void SkinMeshPrimitiveTest::OnBuildRenderablePrimitiveFailed(const IEventPtr& e)
{

}

