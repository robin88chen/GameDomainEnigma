#include "AnimatedModelTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Controllers/InstallingPolicies.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Renderer/RenderablePrimitiveCommands.h"
#include "Renderer/RenderablePrimitiveEvents.h"
#include "Renderer/RendererEvents.h"
#include "CubeGeometryMaker.h"
#include "CameraMaker.h"
#include "ModelPrimitiveMaker.h"
#include "Platforms/AndroidBridge.h"

using namespace Enigma::Controllers;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

AnimatedModelTest::AnimatedModelTest(const std::string app_name) : AppDelegate(app_name)
{
    m_isPrefabBuilt = false;
}

AnimatedModelTest::~AnimatedModelTest()
{

}

void AnimatedModelTest::InitializeMountPaths()
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

void AnimatedModelTest::InstallEngine()
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
    auto prim_policy = ModelPrimitiveMaker::MakeModelPrimitivePolicy("test_model", "test_geometry");
    CommandBus::Post(std::make_shared<Enigma::Renderer::BuildRenderablePrimitive>(prim_policy));

    m_camera = CameraMaker::MakeCamera();
}

void AnimatedModelTest::ShutdownEngine()
{
    m_model = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_camera = nullptr;

    EventPublisher::Unsubscribe(typeid(RenderablePrimitiveBuilt), m_onRenderablePrimitiveBuilt);
    m_onRenderablePrimitiveBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildRenderablePrimitiveFailed);
    m_onBuildRenderablePrimitiveFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void AnimatedModelTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if ((m_renderer) && (m_model))
    {
        m_model->InsertToRendererWithTransformUpdating(m_renderer, Matrix4::IDENTITY, RenderLightingState{});
    }
}

void AnimatedModelTest::RenderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
}

void AnimatedModelTest::OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void AnimatedModelTest::OnRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void AnimatedModelTest::OnRenderablePrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != "test_model") return;
    auto model = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(ev->GetPrimitive());
    if (!m_isPrefabBuilt)
    {
        ModelPrimitiveMaker::SaveModelPrimitiveDto(model, "test_model.model@DataPath");
        auto policy = ModelPrimitiveMaker::LoadModelPrimitivePolicy("test_model.model@DataPath");
        CommandBus::Post(std::make_shared<Enigma::Renderer::BuildRenderablePrimitive>(policy));
        m_isPrefabBuilt = true;
    }
    else
    {
        m_model = model;
    }
}

void AnimatedModelTest::OnBuildRenderablePrimitiveFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("renderable primitive %s build failed : %s\n", ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
}
