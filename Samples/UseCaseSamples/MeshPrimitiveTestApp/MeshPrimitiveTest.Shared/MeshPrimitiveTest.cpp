#include "MeshPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Controllers/InstallingPolicies.h"
#include "CubeGeometryMaker.h"
#include "MeshPrimitiveMaker.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RenderablePrimitiveCommands.h"
#include "Renderer/RenderablePrimitiveEvents.h"
#include "Renderer/MeshPrimitive.h"
#include "GameEngine/Primitive.h"

using namespace Enigma::FileSystem;
using namespace Enigma::Controllers;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

MeshPrimitiveTest::MeshPrimitiveTest(const std::string app_name) : AppDelegate(app_name)
{

}

MeshPrimitiveTest::~MeshPrimitiveTest()
{

}

void MeshPrimitiveTest::InitializeMountPaths()
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

void MeshPrimitiveTest::InstallEngine()
{
    m_onRenderablePrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) {this->OnRenderablePrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onBuildRenderablePrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) {this->OnBuildRenderablePrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));

    CubeGeometryMaker::MakeSavedCube("test_geometry");
    auto prim_policy = MeshPrimitiveMaker::MakeMeshPrimitivePolicy("test_mesh", "test_geometry");
    CommandBus::Post(std::make_shared<Enigma::Renderer::BuildRenderablePrimitive>(std::move(prim_policy)));
}

void MeshPrimitiveTest::ShutdownEngine()
{
    EventPublisher::Unsubscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onRenderablePrimitiveBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);
    m_onBuildRenderablePrimitiveFailed = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void MeshPrimitiveTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
}

void MeshPrimitiveTest::RenderFrame()
{

}

void MeshPrimitiveTest::OnRenderablePrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    MeshPrimitivePtr mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(ev->GetPrimitive());
}

void MeshPrimitiveTest::OnBuildRenderablePrimitiveFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("renderable primitive %s build failed : %s\n", ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
}
