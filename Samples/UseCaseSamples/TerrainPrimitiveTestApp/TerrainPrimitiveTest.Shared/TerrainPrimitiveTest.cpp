#include "TerrainPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "SceneGraph/SceneGraphDtoHelper.h"

using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Gateways;
using namespace Enigma::SceneGraph;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

TerrainPrimitiveTest::TerrainPrimitiveTest(const std::string app_name) : AppDelegate(app_name)
{

}

TerrainPrimitiveTest::~TerrainPrimitiveTest()
{

}

void TerrainPrimitiveTest::InitializeMountPaths()
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

void TerrainPrimitiveTest::InstallEngine()
{
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRendererCreated(e); });
    EventPublisher::Subscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) {this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto default_render_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, default_render_policy });
    CameraDto camera_dto = CameraDtoHelper("camera").EyePosition(Enigma::MathLib::Vector3(-5.0f, 5.0f, -5.0f)).LookAt(Enigma::MathLib::Vector3(1.0f, -1.0f, 1.0f)).UpDirection(Enigma::MathLib::Vector3::UNIT_Y)
        .Frustum("frustum", Frustum::ProjectionType::Perspective).FrustumFov(Enigma::MathLib::Math::PI / 4.0f).FrustumFrontBackZ(0.1f, 100.0f)
        .FrustumNearPlaneDimension(40.0f, 30.0f).ToCameraDto();
    m_camera = std::make_shared<Camera>(camera_dto);
}

void TerrainPrimitiveTest::ShutdownEngine()
{
    m_camera = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;

    EventPublisher::Unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void TerrainPrimitiveTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
}

void TerrainPrimitiveTest::RenderFrame()
{
    if (!m_renderer) return;
    m_renderer->BeginScene();
    m_renderer->ClearRenderTarget();
    m_renderer->DrawScene();
    m_renderer->EndScene();
    m_renderer->Flip();
}

void TerrainPrimitiveTest::OnRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->GetRenderer());
    m_renderer->SetAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}

void TerrainPrimitiveTest::OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->SetRenderTarget(m_renderTarget);
}
