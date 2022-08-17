#include "ShaderManagerTest.h"
#include "MathLib/Vector3.h"
#include "Controllers/InstallingPolicies.h"
#include <cassert>
#include <GraphicKernel/GraphicEvents.h>
#include <GraphicKernel/GraphicCommands.h>
#include <Platforms/MemoryAllocMacro.h>
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ServiceManager.h"
#include "GameEngine/RendererEvents.h"
#include "Frameworks/CommandBus.h"
#include "BufferBuilder.h"
#include "GameEngine/RendererCommands.h"
#include "GameEngine/ShaderCommands.h"
#include "GameEngine/ShaderEvents.h"
#include "GraphicKernel/IVertexShader.h"

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

struct VtxData
{
    Vector3 pos;
    unsigned int idx;
};
static VtxData vtx_pos[] =
{
    { Vector3(-0.25f, 0.25f, 0.5f), 0 },
    { Vector3(0.25f, 0.25f, 0.5f), 1 },
    { Vector3(-0.25f, -0.25f, 0.5f), 2 },
    { Vector3(0.25f, -0.25f, 0.5f), 3 },
};
static unsigned int vtx_idx[] =
{
    0, 1, 2, 1, 3, 2,
};

static std::string vs_code_11 = " \
float4 offset[4]; \
struct VS_INPUT \
{\
    float3 Pos : POSITION;\
    uint Index : BLENDINDICES; \
};\
struct VS_OUTPUT\
{\
    float4 Pos : SV_POSITION;\
    float4 DiffCol : COLOR0;\
};\
VS_OUTPUT vs_main(const VS_INPUT v)\
{\
    VS_OUTPUT o;\
    o.Pos.xyz = v.Pos + offset[v.Index];\
    o.Pos.w = 1.0f; \
    o.DiffCol = float4(0.5f + v.Pos.x + v.Pos.y, 0.5f + v.Pos.x - v.Pos.y, 0.5f + v.Pos.y - v.Pos.x, 1.0f);\
    return o;\
}\
";

static std::string ps_code_11 = ""
"//semantic anim_time ANIM_TIMER\n\r"
"//semantic anim_scale ANIM_SCALE\n\r"
"float anim_time : ANIM_TIMER; \
float anim_scale : ANIM_SCALE; \
struct PS_INPUT \
{ \
    float4 Pos : SV_POSITION; \
    float4 DiffCol : COLOR0; \
}; \
struct PS_OUTPUT \
{ \
    float4 Color : SV_TARGET; \
}; \
PS_OUTPUT ps_main(const PS_INPUT p) \
{ \
    PS_OUTPUT o; \
    o.Color = p.DiffCol; \
    return o; \
} \
";

static const std::string vs_code_egl =
"#version 300 es\n"
"uniform vec4 offset[4];\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in uint idx;\n"
"out vec4 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(pos, 1.0) + offset[idx];\n"
"   vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);\n"
"}\n";

static const std::string ps_code_egl =
"#version 300 es\n"
"//semantic anim_time ANIM_TIMER\n"
"//semantic anim_scale ANIM_SCALE\n"
"precision mediump float;\n"
"uniform float anim_time;\n"
"uniform float anim_scale;\n"
"in vec4 vColor;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor = (0.5f + cos(anim_time * anim_scale)/2.0f) * vColor;\n"
"}\n";

ShaderManagerTest::ShaderManagerTest(const std::string app_name) : AppDelegate(app_name)
{
}

ShaderManagerTest::~ShaderManagerTest()
{
}

void ShaderManagerTest::InstallEngine()
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

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();

#if TARGET_PLATFORM == PLATFORM_WIN32
    CommandBus::Post(ICommandPtr{ menew BuildShaderProgram{ { ShaderProgramName, "vtx_shader", "xyzb1_betabyte", vs_code_11, "vs_4_0", "vs_main",VertexDeclName, "pix_shader", ps_code_11, "ps_4_0", "ps_main" }} });
#else
    CommandBus::Post(ICommandPtr{ menew BuildShaderProgram{ { ShaderProgramName, "vtx_shader", "xyzb1_betabyte", vs_code_egl, "vs_4_0", "vs_main",VertexDeclName, "pix_shader", ps_code_egl, "ps_4_0", "ps_main" }} });
#endif
    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder();
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);
}

void ShaderManagerTest::ShutdownEngine()
{
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
	
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

    m_renderTarget = nullptr;
    m_program = nullptr;
    m_vtxDecl = nullptr;
    m_vtxBuffer = nullptr;
    m_idxBuffer = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void ShaderManagerTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
}

void ShaderManagerTest::RenderFrame()
{
    if ((!m_vtxDecl) || (!m_program) || (!m_vtxBuffer) || (!m_idxBuffer) || (!m_renderTarget)) return;
#if TARGET_PLATFORM == PLATFORM_WIN32
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    IGraphicAPI::Instance()->AsyncBindVertexDeclaration(m_vtxDecl);
    IGraphicAPI::Instance()->AsyncBindShaderProgram(m_program);
    IGraphicAPI::Instance()->AsyncBindVertexBuffer(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->AsyncBindIndexBuffer(m_idxBuffer);
    m_renderTarget->Clear();
    CommandBus::Post(ICommandPtr{ menew BeginScene });
    CommandBus::Post(ICommandPtr{ menew DrawIndexedPrimitive{6, 4, 0, 0} });
    CommandBus::Post(ICommandPtr{ menew EndScene });
    m_renderTarget->Flip();
#else
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    IGraphicAPI::Instance()->BindVertexDeclaration(m_vtxDecl);
    IGraphicAPI::Instance()->BindShaderProgram(m_program);
    IGraphicAPI::Instance()->BindVertexBuffer(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->BindIndexBuffer(m_idxBuffer);
    m_renderTarget->Clear();
    CommandBus::Post(ICommandPtr{ menew BeginScene });
    CommandBus::Post(ICommandPtr{ menew DrawIndexedPrimitive{6, 4, 0, 0} });
    CommandBus::Post(ICommandPtr{ menew EndScene });
    m_renderTarget->Flip();
#endif
}

void ShaderManagerTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}

void ShaderManagerTest::OnShaderProgramBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<ShaderProgramBuilt> ev = std::dynamic_pointer_cast<ShaderProgramBuilt, IEvent>(e);
    if ((!ev) || (!ev->GetProgram())) return;
    if (ev->GetShaderName() != ShaderProgramName) return;
    m_program = ev->GetProgram();
    m_vtxDecl = m_program->GetVertexDeclaration();
}

void ShaderManagerTest::OnShaderProgramBuildFailed(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<ShaderProgramBuildFailed> ev = std::dynamic_pointer_cast<ShaderProgramBuildFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("shader program %s build failed : %s\n", ev->GetShaderName().c_str(), ev->GetErrorCode().message().c_str());
}

void ShaderManagerTest::OnVertexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<VertexBufferBuilt> ev = std::dynamic_pointer_cast<VertexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != VertexBufferName) return;
    m_vtxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IVertexBufferPtr>(ev->GetBufferName());
}

void ShaderManagerTest::OnIndexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<IndexBufferBuilt> ev = std::dynamic_pointer_cast<IndexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != IndexBufferName) return;
    m_idxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IIndexBufferPtr>(ev->GetBufferName());
}
