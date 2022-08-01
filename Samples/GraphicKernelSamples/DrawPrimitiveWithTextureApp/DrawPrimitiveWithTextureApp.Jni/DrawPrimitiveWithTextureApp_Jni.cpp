#include "DrawPrimitiveWithTextureApp_Jni.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Controllers/InstallingPolicies.h"
#include <cassert>
#include <GraphicKernel/GraphicEvents.h>
#include <Platforms/MemoryAllocMacro.h>
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ServiceManager.h"
#include "GameEngine/RendererEvents.h"
#include "Frameworks/CommandBus.h"
#include "ShaderBuilder.h"
#include "BufferBuilder.h"
#include "GameEngine/RendererCommands.h"
#include "TextureSamplerBuilder.h"
#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IShaderVariable.h"

using namespace Enigma::Controllers;
using namespace Enigma::Graphics;
using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string ShaderProgramName = "shader_program";
std::string VertexDeclName = "vtx_layout";
std::string VertexBufferName = "vtx_buffer";
std::string IndexBufferName = "idx_buffer";
std::string TextureName = "earth";
std::string SamplerName = "sampler";

struct VtxData
{
    Vector3 pos;
    unsigned int idx;
    Vector2 coord;
};
static VtxData vtx_pos[] =
{
    { Vector3(-0.25f, 0.25f, 0.5f), 0, Vector2(0.0f, 0.0f)},
    { Vector3(0.25f, 0.25f, 0.5f), 1, Vector2(1.0f, 0.0f) },
    { Vector3(-0.25f, -0.25f, 0.5f), 2, Vector2(0.0f, 1.0f) },
    { Vector3(0.25f, -0.25f, 0.5f), 3, Vector2(1.0f, 1.0f) },
};
static unsigned int vtx_idx[] =
{
    0, 1, 2, 1, 3, 2,
};

static const char VERTEX_SHADER[] =
"#version 300 es\n"
"uniform vec4 offset[4];\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in uint idx;\n"
"layout(location = 2) in vec2 texco;\n"
"out vec2 vTexCo;\n"
"out vec4 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(pos, 1.0) + offset[idx];\n"
"    vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);\n"
"    vTexCo = texco;\n"
"}\n";

static const char FRAGMENT_SHADER[] =
"#version 300 es\n"
"//semantic anim_time ANIM_TIMER\n"
"//semantic anim_scale ANIM_SCALE\n"
"precision mediump float;\n"
"//sampler_state samLinear DiffuseTexture\n"
"uniform sampler2D DiffuseTexture;\n"
"uniform float anim_time;\n"
"uniform float anim_scale;\n"
"in vec4 vColor;\n"
"in vec2 vTexCo;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor = (0.5f + cos(anim_time * anim_scale)/2.0f) * vColor;\n"
"    outColor = texture(DiffuseTexture, vTexCo) * outColor;\n"
"}\n";

static Vector4 vtx_offset[4] =
{
    Vector4(-0.3f, 0.0f, 0.0f, 0.0f),
    Vector4(0.1f, 0.0f, 0.0f, 0.0f),
    Vector4(0.1f, 0.0f, 0.0f, 0.0f),
    Vector4(-0.1f, 0.0f, 0.0f, 0.0f),
};

DrawPrimitiveWithTextureApp::DrawPrimitiveWithTextureApp(const std::string app_name) : AppDelegate(app_name)
{
}

DrawPrimitiveWithTextureApp::~DrawPrimitiveWithTextureApp()
{
}

void DrawPrimitiveWithTextureApp::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onShaderProgramCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceShaderProgramCreated), m_onShaderProgramCreated);
    m_onVertexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onIndexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnIndexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onSamplerBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSamplerBuilt(e); });
    EventPublisher::Subscribe(typeid(SamplerStateBuilt), m_onSamplerBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(IGraphicAPI::Instance(), DeviceRequiredBits(), nullptr);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();

    m_shaderBuilder = menew ShaderBuilder();
    m_shaderBuilder->BuildShaderProgram(ShaderBuilder::ShaderProgramBuildParameter{ ShaderProgramName, "vtx_shader", "xyzb1_tex1(2)_betabyte", VERTEX_SHADER, VertexDeclName, "pix_shader", FRAGMENT_SHADER });

    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder();
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);

    m_textureBuilder = menew TextureSamplerBuilder();
    m_textureBuilder->BuildTexture(TextureName, "earth.png", AndroidAssetPathName);
    IDeviceSamplerState::SamplerStateData samp_data;
    samp_data.m_addressModeU = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    samp_data.m_addressModeV = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    m_textureBuilder->BuildSampler(SamplerName, samp_data);

    m_timer = menew Timer;
    m_timer->Reset();
    m_tick = 0.0f;
}

void DrawPrimitiveWithTextureApp::ShutdownEngine()
{
    delete m_shaderBuilder;
    m_shaderBuilder = nullptr;
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
    delete m_textureBuilder;
    m_textureBuilder = nullptr;
    delete m_timer;
    m_timer = nullptr;

    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceShaderProgramCreated), m_onShaderProgramCreated);
    m_onShaderProgramCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onVertexBufferBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);
    m_onIndexBufferBuilt = nullptr;

    EventPublisher::Unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    EventPublisher::Unsubscribe(typeid(SamplerStateBuilt), m_onSamplerBuilt);
    m_onSamplerBuilt = nullptr;

    m_renderTarget = nullptr;
    m_program = nullptr;
    m_vtxDecl = nullptr;
    m_vtxBuffer = nullptr;
    m_idxBuffer = nullptr;
    m_texture = nullptr;
    m_sampler = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void DrawPrimitiveWithTextureApp::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (m_timer)
    {
        m_timer->Update();
        m_tick += m_timer->GetElapseTime();
    }
    if (m_program)
    {
        IShaderVariablePtr ps_var = m_program->GetVariableBySemantic("ANIM_TIMER");
        ps_var->SetValue(m_tick);
    }
}

void DrawPrimitiveWithTextureApp::RenderFrame()
{
    if ((!m_vtxDecl) || (!m_program) || (!m_vtxBuffer) || (!m_idxBuffer) || (!m_renderTarget)) return;
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    IGraphicAPI::Instance()->BindVertexDeclaration(m_vtxDecl);
    IGraphicAPI::Instance()->BindShaderProgram(m_program);
    m_program->ApplyVariables();
    IGraphicAPI::Instance()->BindVertexBuffer(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->BindIndexBuffer(m_idxBuffer);
    m_renderTarget->Clear();
    IGraphicAPI::Instance()->BeginScene();
    IGraphicAPI::Instance()->DrawIndexedPrimitive(6, 4, 0, 0);
    IGraphicAPI::Instance()->EndScene();
    m_renderTarget->Flip();
}

void DrawPrimitiveWithTextureApp::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}

void DrawPrimitiveWithTextureApp::OnShaderProgramCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<DeviceShaderProgramCreated> ev = std::dynamic_pointer_cast<DeviceShaderProgramCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != ShaderProgramName) return;
    m_program = IGraphicAPI::Instance()->GetGraphicAsset<IShaderProgramPtr>(ev->GetName());
    m_vtxDecl = IGraphicAPI::Instance()->GetGraphicAsset<IVertexDeclarationPtr>(VertexDeclName);
    BuildVariables();
}

void DrawPrimitiveWithTextureApp::OnVertexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<VertexBufferBuilt> ev = std::dynamic_pointer_cast<VertexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != VertexBufferName) return;
    m_vtxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IVertexBufferPtr>(ev->GetBufferName());
}

void DrawPrimitiveWithTextureApp::OnIndexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<IndexBufferBuilt> ev = std::dynamic_pointer_cast<IndexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != IndexBufferName) return;
    m_idxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IIndexBufferPtr>(ev->GetBufferName());
}

void DrawPrimitiveWithTextureApp::OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<TextureLoaded> ev = std::dynamic_pointer_cast<TextureLoaded, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != TextureName) return;
    m_texture = IGraphicAPI::Instance()->GetGraphicAsset<ITexturePtr>(ev->GetTextureName());
    BuildVariables();
}

void DrawPrimitiveWithTextureApp::OnSamplerBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<SamplerStateBuilt> ev = std::dynamic_pointer_cast<SamplerStateBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSamplerName() != SamplerName) return;
    m_sampler = IGraphicAPI::Instance()->GetGraphicAsset<IDeviceSamplerStatePtr>(ev->GetSamplerName());
    BuildVariables();
}

void DrawPrimitiveWithTextureApp::BuildVariables()
{
    if ((!m_program) || (!m_texture) || (!m_sampler)) return;

    IShaderVariablePtr ps_var = m_program->GetVariableByName("DiffuseTexture");
    ps_var->SetValue(IShaderVariable::TextureVarTuple{ m_texture, std::nullopt });
    IShaderVariablePtr samp_var = m_program->GetVariableByName("samLinear");
    if (samp_var)
    {
        samp_var->SetValue(m_sampler);
    }
    auto offset_var = m_program->GetVariableByName("offset[0]");
    offset_var->SetValues(vtx_offset, 4);
}
