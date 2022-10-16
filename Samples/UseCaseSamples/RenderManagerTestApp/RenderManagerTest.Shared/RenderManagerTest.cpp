#include "RenderManagerTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/TextureEvents.h"
#include "Gateways/EffectPolicyJsonGateway.h"
#include "Controllers/InstallingPolicies.h"
#include "Platforms/MemoryAllocMacro.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "GameEngine/RenderBufferSignature.h"
#include "GameEngine/RenderBufferBuildingPolicies.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/TextureCommands.h"
#include "Renderer/RendererManager.h"
#include "FileSystem/AndroidMountPath.h"
#include <filesystem>
#include <memory>

using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::Controllers;
using namespace Enigma::MathLib;
using namespace Enigma::Graphics;
using namespace Enigma::Renderer;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string MaterialName = "test_effect";
std::string VertexBufferName = "vtx_buffer";
std::string IndexBufferName = "idx_buffer";
std::string RenderBufferName = "render_buffer";
std::string BufferSignatureName = "signature";
std::string TextureName = "earth";
std::string PassTopLeftName = "pass_top_left";
std::string PassTopRightName = "pass_top_right";
std::string PassBottomLeftName = "pass_bottom_left";
std::string PassBottomRightName = "pass_bottom_right";

struct VtxData
{
    Vector3 pos;
    Vector2 coord;
};
static VtxData vtx_pos[] =
{
    { Vector3(-0.25f, 0.25f, 0.5f), Vector2(0.0f, 0.0f)},
    { Vector3(0.25f, 0.25f, 0.5f), Vector2(1.0f, 0.0f) },
    { Vector3(-0.25f, -0.25f, 0.5f), Vector2(0.0f, 1.0f) },
    { Vector3(0.25f, -0.25f, 0.5f), Vector2(1.0f, 1.0f) },
};
static unsigned int vtx_idx[] =
{
    0, 1, 2, 1, 3, 2,
};

RenderManagerTest::RenderManagerTest(const std::string app_name) : AppDelegate(app_name)
{

}

RenderManagerTest::~RenderManagerTest()
{

}

void RenderManagerTest::InitializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::Instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../../Media/";
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    if (FileSystem::Instance())
    {
        FileSystem::Instance()->AddMountPath(std::make_shared<AndroidMountPath>("", MediaPathName));
    }
#endif
}

void RenderManagerTest::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectFailed(e); });
    EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectFailed);

    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildRenderBufferFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadTextureFailed(e); });
    EventPublisher::Subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    //m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();
    m_materialManager = ServiceManager::GetSystemServiceAs<EffectMaterialManager>();

    m_gateway = menew EffectPolicyJsonGateway();

    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("TestEffect.efx@APK_PATH"), "rb");
    if (FATAL_LOG_EXPR(!iFile)) return;
    size_t file_size = iFile->Size();
    if (FATAL_LOG_EXPR(file_size <= 0))
    {
        FileSystem::Instance()->CloseFile(iFile);
        return;
    }

    auto code_buff = iFile->Read(0, file_size);
    if (!code_buff) return;
    FileSystem::Instance()->CloseFile(iFile);
    auto compiling_policy = m_gateway->Deserialize(convert_to_string(code_buff.value(), file_size));
    if (compiling_policy)
    {
        m_materialManager->CompileEffectMaterial(compiling_policy.value());
    }
    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    RenderBufferSignature signature{ BufferSignatureName, PrimitiveTopology::Topology_TriangleList, 4, 6 };
    RenderBufferPolicy buffer_policy{ RenderBufferName, signature, VertexBufferName, IndexBufferName,
        sizeof(VtxData), sizeof(vtx_pos), sizeof(vtx_idx), points, indices };
    CommandBus::Post(std::make_shared<BuildRenderBuffer>(buffer_policy));

    CommandBus::Post(std::make_shared<LoadTexture>(TexturePolicy(TextureName, "earth.png", MediaPathName)));

    m_timer = menew Timer;
    m_timer->Reset();
    m_tick = 0.0f;
}

void RenderManagerTest::ShutdownEngine()
{
    delete m_gateway;
    m_gateway = nullptr;
    delete m_timer;
    m_timer = nullptr;

    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::Unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectFailed);
    m_onCompileEffectFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    EventPublisher::Unsubscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;

    m_renderTarget = nullptr;
    m_renderBuffer = nullptr;
    m_texture = nullptr;

    m_material = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}
void RenderManagerTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (m_timer)
    {
        m_timer->Update();
        m_tick += m_timer->GetElapseTime();
    }
    if (m_material)
    {
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassTopRightName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassTopRightName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(3.0f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassBottomLeftName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(-m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassBottomLeftName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(1.5f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassBottomRightName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassBottomRightName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(3.0f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassTopLeftName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(-m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassTopLeftName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(1.5f);
        }
    }
}

void RenderManagerTest::RenderFrame()
{
    if ((!m_material) || (!m_renderBuffer) || (!m_renderTarget) || (!m_texture)) return;
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    m_renderTarget->Clear();
    IGraphicAPI::Instance()->BeginScene();
    m_material->SelectRendererTechnique("technique");
    m_material->SelectVisualTechnique("slash");
    //m_material->SelectVisualTechnique("backslash");
    m_renderBuffer->Draw(m_material, GeometrySegment(0, 4, 0, 6));
    IGraphicAPI::Instance()->EndScene();
    m_renderTarget->Flip();
}

void RenderManagerTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->GetRenderTarget();
}

void RenderManagerTest::OnEffectMaterialCompiled(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<EffectMaterialCompiled> ev = std::dynamic_pointer_cast<EffectMaterialCompiled, IEvent>(e);
    if ((!ev) || (!ev->HasEffect())) return;
    if (ev->GetFilename() != MaterialName) return;
    m_material = ev->GetEffect();
    BuildVariables();
}

void RenderManagerTest::OnCompileEffectFailed(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<CompileEffectMaterialFailed> ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("effect material %s compile failed : %s\n", ev->GetFilename().c_str(), ev->GetErrorCode().message().c_str());
}

void RenderManagerTest::OnRenderBufferBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (ev->GetName() != RenderBufferName) return;
    m_renderBuffer = ev->GetBuffer();
}

void RenderManagerTest::OnBuildRenderBufferFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("render buffer %s build failed : %s\n", ev->GetName().c_str(),
        ev->GetErrorCode().message().c_str());
}

void RenderManagerTest::OnTextureLoaded(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<TextureLoaded> ev = std::dynamic_pointer_cast<TextureLoaded, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != TextureName) return;
    m_texture = ev->GetTexture();
    BuildVariables();
}

void RenderManagerTest::OnLoadTextureFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LoadTextureFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("load texture %s failed : %s\n", ev->GetTextureName().c_str(),
        ev->GetError().message().c_str());
}

void RenderManagerTest::BuildVariables()
{
    if ((!m_material) || (!m_texture)) return;

    auto vars = m_material->GetEffectVariablesByName("DiffuseTexture");
    if (!vars.empty())
    {
        for (auto& var : vars)
        {
            var.get().AssignValue(IShaderVariable::TextureVarTuple{ m_texture->GetDeviceTexture(), std::nullopt });
        }
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopLeftName, "offset_x"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopLeftName, "offset_y"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomLeftName, "offset_x"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomLeftName, "offset_y"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopRightName, "offset_x"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopRightName, "offset_y"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomRightName, "offset_x"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomRightName, "offset_y"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
}
