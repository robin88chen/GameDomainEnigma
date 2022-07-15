#include "DrawPrimitiveWithTexture.h"
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
"struct VS_INPUT \n"
"{\n"
"	float3 Pos : POSITION;\n"
"	float2 Coord : TEXCOORD0;\n"
"};\n"
"struct VS_OUTPUT\n"
"{\n"
"	float4 Pos : SV_POSITION;\n"
"	float2 Coord : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main(const VS_INPUT v)\n"
"{\n"
"	VS_OUTPUT o = (VS_OUTPUT)0;"
"	o.Pos.xyz = v.Pos;\n"
" o.Pos.w = 1.0f;\n"
"	o.Coord = v.Coord;\n"
"	return o;\n"
"};\n"
"";
static std::string ps_code_11 =
"//semantic anim_time ANIM_TIMER\n\r"
"Texture2D DiffuseTexture : DiffuseMap;\n"
"SamplerState samLinear\n"
"{\n"
"	Filter = MIN_MAG_MIP_LINEAR;\n"
"	AddressU = Wrap;\n"
"	AddressV = Wrap;\n"
"};\n"
"float anim_time : ANIM_TIMER;\n"
"struct PS_INPUT\n"
"{\n"
"	float4 Pos : SV_POSITION;\n"
"	float2 Coord : TEXCOORD0;\n"
"};\n"
"float4 ps_main(PS_INPUT input) : SV_TARGET\n"
"{\n"
" float2 co = float2(input.Coord.x + anim_time, input.Coord.y);\n"
"	float4 outputColor = DiffuseTexture.Sample(samLinear, co);\n"
" outputColor.a = 1;\n"
"	return outputColor;\n"
"};\n";

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

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(IGraphicAPI::Instance(), DeviceRequiredBits(), m_asyncType, m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();

    m_shaderBuilder = menew ShaderBuilder(m_asyncType);
    m_shaderBuilder->BuildShaderProgram(ShaderBuilder::ShaderProgramBuildParameter{ ShaderProgramName, "vtx_shader", "xyz_tex1(2)", vs_code_11, VertexDeclName, "pix_shader", ps_code_11 });

    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder(m_asyncType);
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);

    m_textureBuilder = menew TextureSamplerBuilder(m_asyncType);
    m_textureBuilder->BuildTexture(TextureName, "earth.bmp", "");
    IDeviceSamplerState::SamplerStateData samp_data;
    samp_data.m_addressModeU = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    samp_data.m_addressModeV = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    m_textureBuilder->BuildSampler(SamplerName, samp_data);
}

void DrawPrimitiveWithTextureApp::ShutdownEngine()
{
    delete m_shaderBuilder;
    m_shaderBuilder = nullptr;
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
    delete m_textureBuilder;
    m_textureBuilder = nullptr;

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
}

void DrawPrimitiveWithTextureApp::RenderFrame()
{
    if ((!m_vtxDecl) || (!m_program) || (!m_vtxBuffer) || (!m_idxBuffer) || (!m_renderTarget)) return;
    m_renderTarget->AsyncBind();
    m_renderTarget->AsyncBindViewPort();
    IGraphicAPI::Instance()->AsyncBindVertexDeclaration(m_vtxDecl);
    IGraphicAPI::Instance()->AsyncBindShaderProgram(m_program);
    m_program->AsyncApplyVariables();
    IGraphicAPI::Instance()->AsyncBindVertexBuffer(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->AsyncBindIndexBuffer(m_idxBuffer);
    m_renderTarget->AsyncClear();
    IGraphicAPI::Instance()->AsyncBeginScene();
    IGraphicAPI::Instance()->AsyncDrawIndexedPrimitive(6, 4, 0, 0);
    IGraphicAPI::Instance()->AsyncEndScene();
    m_renderTarget->AsyncFlip();
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
}
