#include "EffectPassTest.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/Command.h"
#include "GameEngine/RendererEvents.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererCommands.h"
#include "GameEngine/ShaderCommands.h"
#include "Frameworks/ServiceManager.h"
#include "Controllers/InstallingPolicies.h"
#include "GameEngine/ShaderBuildingPolicies.h"
#include "GameEngine/ShaderEvents.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicCommands.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Platforms/MemoryAllocMacro.h"
#include "BufferBuilder.h"
#include "TextureSamplerBuilder.h"

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

static std::string vs_code_11 =
"float offset;\n"
"struct VS_INPUT \n"
"{\n"
"   float3 Pos : POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"};\n"
"struct VS_OUTPUT\n"
"{\n"
"   float4 Pos : SV_POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"   float4 DiffCol : COLOR0; \n"
"};\n"
"VS_OUTPUT vs_main(const VS_INPUT v)\n"
"{\n"
"   VS_OUTPUT o = (VS_OUTPUT)0;"
"   o.Pos.xyz = v.Pos + float3(0.0f, offset, 0.0f);\n"
" o.Pos.w = 1.0f;\n"
"   o.Coord = v.Coord;\n"
"   o.DiffCol = float4(0.5f + v.Pos.x + v.Pos.y, 0.5f + v.Pos.x - v.Pos.y, 0.5f + v.Pos.y - v.Pos.x, 1.0f); \n"
"   return o;\n"
"};\n"
"";

static std::string ps_code_11 =
"//semantic anim_time ANIM_TIMER\n\r"
"//semantic anim_scale ANIM_SCALE\n\r"
"Texture2D DiffuseTexture : DiffuseMap;\n"
"SamplerState samLinear\n"
"{\n"
"   Filter = MIN_MAG_MIP_LINEAR;\n"
"   AddressU = Wrap;\n"
"   AddressV = Wrap;\n"
"};\n"
"float anim_time : ANIM_TIMER;\n"
"float anim_scale : ANIM_SCALE; \n"
"struct PS_INPUT\n"
"{\n"
"   float4 Pos : SV_POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"   float4 DiffCol : COLOR0; \n"
"};\n"
"float4 ps_main(PS_INPUT input) : SV_TARGET\n"
"{\n"
" float2 co = float2(input.Coord.x + anim_time, input.Coord.y);\n"
"   float4 outputColor = (0.5f + cos(anim_time * anim_scale) / 2.0f) * input.DiffCol;\n"
"   outputColor = DiffuseTexture.Sample(samLinear, co) * outputColor;\n"
"   outputColor.a = 1;\n"
"   return outputColor;\n"
"};\n";

static const std::string vs_code_egl =
"#version 300 es\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in vec2 texco;\n"
"out vec2 vTexCo;\n"
"out vec4 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(pos, 1.0);\n"
"    vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);\n"
"    vTexCo = texco;\n"
"}\n";

static const std::string ps_code_egl =
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

EffectPassTest::EffectPassTest(const std::string app_name) : AppDelegate(app_name)
{
    m_effectPass = nullptr;
    m_effectPass2 = nullptr;
}

EffectPassTest::~EffectPassTest()
{
	
}

void EffectPassTest::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onShaderProgramBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramBuilt(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onShaderProgramBuildFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramBuildFailed(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuildFailed), m_onShaderProgramBuildFailed);
    m_onVertexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onIndexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnIndexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);

	m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onSamplerBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSamplerBuilt(e); });
    EventPublisher::Subscribe(typeid(SamplerStateBuilt), m_onSamplerBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();

#if TARGET_PLATFORM == PLATFORM_WIN32
    CommandBus::Post(std::make_shared<BuildShaderProgram>(ShaderProgramPolicy{
        ShaderProgramName, "vtx_shader", "xyz_tex1(2)", vs_code_11, "vs_4_0", "vs_main", VertexDeclName, "pix_shader", ps_code_11, "ps_4_0", "ps_main" }));
#else
    CommandBus::Post(std::make_shared<BuildShaderProgram>(ShaderProgramPolicy{
        ShaderProgramName, "vtx_shader", "xyz_tex1(2)", vs_code_egl, "vs_4_0", "vs_main",VertexDeclName, "pix_shader", ps_code_egl, "ps_4_0", "ps_main" }));
#endif
    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder();
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);

	m_textureBuilder = menew TextureSamplerBuilder();
    m_textureBuilder->BuildTexture(TextureName, "earth.png", "");
    IDeviceSamplerState::SamplerStateData samp_data;
    samp_data.m_addressModeU = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    samp_data.m_addressModeV = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    m_textureBuilder->BuildSampler(SamplerName, samp_data);

	m_timer = menew Timer;
    m_timer->Reset();
    m_tick = 0.0f;
}

void EffectPassTest::ShutdownEngine()
{
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
    delete m_textureBuilder;
    m_textureBuilder = nullptr;
    delete m_timer;
    m_timer = nullptr;

    if (m_effectPass) delete m_effectPass;
    m_effectPass = nullptr;
    if (m_effectPass2) delete m_effectPass2;
    m_effectPass2 = nullptr;

    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onShaderProgramBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuildFailed), m_onShaderProgramBuildFailed);
    m_onShaderProgramBuildFailed = nullptr;
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
void EffectPassTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (m_timer)
    {
        m_timer->Update();
        m_tick += m_timer->GetElapseTime();
    }
    if (m_effectPass)
    {
        auto var = m_effectPass->GetVariableBySemantic("ANIM_TIMER");
        if (var) var.value().get().AssignValue(m_tick);
        auto var1 = m_effectPass->GetVariableBySemantic("ANIM_SCALE");
        if (var1) var1.value().get().AssignValue(3.0f);
    }
    if (m_effectPass2)
    {
        auto var = m_effectPass2->GetVariableBySemantic("ANIM_TIMER");
        if (var) var.value().get().AssignValue(-m_tick);
        auto var1 = m_effectPass2->GetVariableBySemantic("ANIM_SCALE");
        if (var1) var1.value().get().AssignValue(1.5f);
    }
}

void EffectPassTest::RenderFrame()
{
    if ((!m_vtxDecl) || (!m_program) || (!m_vtxBuffer) || (!m_idxBuffer) || (!m_renderTarget)) return;
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    IGraphicAPI::Instance()->Bind(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->Bind(m_idxBuffer);
    //CommandBus::Post(std::make_shared<BindVertexBuffer>(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList));
    //CommandBus::Post(std::make_shared<BindIndexBuffer>(m_idxBuffer));
    m_renderTarget->Clear();
    IGraphicAPI::Instance()->BeginScene();
    //CommandBus::Post(std::make_shared<BeginScene>());
    m_effectPass->CommitVariables();
    m_effectPass->Apply();
    IGraphicAPI::Instance()->Draw(6, 4, 0, 0);
    //CommandBus::Post(std::make_shared<DrawIndexedPrimitive>(6, 4, 0, 0));
    m_effectPass2->CommitVariables();
    m_effectPass2->Apply();
    IGraphicAPI::Instance()->Draw(6, 4, 0, 0);
    IGraphicAPI::Instance()->EndScene();
    //CommandBus::Post(std::make_shared<DrawIndexedPrimitive>(6, 4, 0, 0));
    //CommandBus::Post(std::make_shared<EndScene>());
    m_renderTarget->Flip();
}

void EffectPassTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}

void EffectPassTest::OnShaderProgramBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<ShaderProgramBuilt> ev = std::dynamic_pointer_cast<ShaderProgramBuilt, IEvent>(e);
    if ((!ev) || (!ev->GetProgram())) return;
    if (ev->GetShaderName() != ShaderProgramName) return;
    m_program = ev->GetProgram();
    m_vtxDecl = m_program->GetVertexDeclaration();
    EffectPass p{ m_program, EffectPassStates{} };  // testing copy constructor
    m_effectPass = menew EffectPass(p);
    m_effectPass2 = menew EffectPass(*m_effectPass);
    BuildVariables();
}

void EffectPassTest::OnShaderProgramBuildFailed(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<ShaderProgramBuildFailed> ev = std::dynamic_pointer_cast<ShaderProgramBuildFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("shader program %s build failed : %s\n", ev->GetShaderName().c_str(), ev->GetErrorCode().message().c_str());
}

void EffectPassTest::OnVertexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<VertexBufferBuilt> ev = std::dynamic_pointer_cast<VertexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != VertexBufferName) return;
    m_vtxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IVertexBufferPtr>(ev->GetBufferName());
}

void EffectPassTest::OnIndexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<IndexBufferBuilt> ev = std::dynamic_pointer_cast<IndexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != IndexBufferName) return;
    m_idxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IIndexBufferPtr>(ev->GetBufferName());
}

void EffectPassTest::OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<TextureLoaded> ev = std::dynamic_pointer_cast<TextureLoaded, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != TextureName) return;
    m_texture = IGraphicAPI::Instance()->GetGraphicAsset<ITexturePtr>(ev->GetTextureName());
    BuildVariables();
}

void EffectPassTest::OnSamplerBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<SamplerStateBuilt> ev = std::dynamic_pointer_cast<SamplerStateBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSamplerName() != SamplerName) return;
    m_sampler = IGraphicAPI::Instance()->GetGraphicAsset<IDeviceSamplerStatePtr>(ev->GetSamplerName());
    BuildVariables();
}

void EffectPassTest::BuildVariables()
{
    if ((!m_effectPass) || (!m_texture) || (!m_sampler)) return;

    auto var = m_effectPass->GetVariableByName("DiffuseTexture");
    if (var)
    {
        var.value().get().AssignValue(IShaderVariable::TextureVarTuple{ m_texture, std::nullopt });
    }
    auto samp_var = m_effectPass->GetVariableByName("samLinear");
    if (samp_var)
    {
        samp_var.value().get().AssignValue(m_sampler);
    }
    auto offset_var = m_effectPass->GetVariableByName("offset");
    if (offset_var)
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    auto var2 = m_effectPass2->GetVariableByName("DiffuseTexture");
    if (var2)
    {
        var2.value().get().AssignValue(IShaderVariable::TextureVarTuple{ m_texture, std::nullopt });
    }
    auto samp_var2 = m_effectPass2->GetVariableByName("samLinear");
    if (samp_var2)
    {
        samp_var2.value().get().AssignValue(m_sampler);
    }
    auto offset_var2 = m_effectPass2->GetVariableByName("offset");
    if (offset_var2)
    {
        offset_var2.value().get().AssignValue(0.3f);
    }
}
