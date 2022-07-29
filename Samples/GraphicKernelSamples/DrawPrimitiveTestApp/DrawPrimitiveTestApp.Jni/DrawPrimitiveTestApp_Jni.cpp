#include "DrawPrimitiveTestApp_Jni.h"
#include "MathLib/Vector3.h"
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

static Vector4 vtx_offset[4] =
{
    Vector4(-0.1f, 0.0f, 0.0f, 0.0f),
    Vector4(0.1f, 0.0f, 0.0f, 0.0f),
    Vector4(0.1f, 0.0f, 0.0f, 0.0f),
    Vector4(-0.1f, 0.0f, 0.0f, 0.0f),
};

static const std::string VERTEX_SHADER =
"#version 300 es\n"
"uniform vec4 offset[4];\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in uint idx;\n"
"out vec4 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(pos, 1.0) + offset[idx];\n"
"   vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);\n"
"}\n";

static const std::string FRAGMENT_SHADER =
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

DrawPrimitiveTestApp::DrawPrimitiveTestApp(const std::string app_name) : AppDelegate(app_name)
{
}

DrawPrimitiveTestApp::~DrawPrimitiveTestApp()
{
}

void DrawPrimitiveTestApp::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onShaderProgramCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceShaderProgramCreated), m_onShaderProgramCreated);
    m_onVertexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onIndexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnIndexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(IGraphicAPI::Instance(), DeviceRequiredBits(), nullptr);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();

    m_shaderBuilder = menew ShaderBuilder();
    m_shaderBuilder->BuildShaderProgram(ShaderBuilder::ShaderProgramBuildParameter{ ShaderProgramName, "vtx_shader", "xyzb1_betabyte", VERTEX_SHADER, VertexDeclName, "pix_shader", FRAGMENT_SHADER });

    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder();
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);
    m_timer = menew Timer;
    m_timer->Reset();
    m_tick = 0.0f;
}

void DrawPrimitiveTestApp::ShutdownEngine()
{
    delete m_shaderBuilder;
    m_shaderBuilder = nullptr;
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
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

    m_renderTarget = nullptr;
    m_program = nullptr;
    m_vtxDecl = nullptr;
    m_vtxBuffer = nullptr;
    m_idxBuffer = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void DrawPrimitiveTestApp::FrameUpdate()
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
        IShaderVariablePtr ps_var1 = m_program->GetVariableBySemantic("ANIM_SCALE");
        ps_var1->SetValue(3.0f);
    }
}

void DrawPrimitiveTestApp::RenderFrame()
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

void DrawPrimitiveTestApp::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}

void DrawPrimitiveTestApp::OnShaderProgramCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<DeviceShaderProgramCreated> ev = std::dynamic_pointer_cast<DeviceShaderProgramCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != ShaderProgramName) return;
    m_program = IGraphicAPI::Instance()->GetGraphicAsset<IShaderProgramPtr>(ev->GetName());
    m_vtxDecl = IGraphicAPI::Instance()->GetGraphicAsset<IVertexDeclarationPtr>(VertexDeclName);

    auto shader_var = m_program->GetVariableByName("offset[0]");
    shader_var->SetValues(vtx_offset, 4);
}

void DrawPrimitiveTestApp::OnVertexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<VertexBufferBuilt> ev = std::dynamic_pointer_cast<VertexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != VertexBufferName) return;
    m_vtxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IVertexBufferPtr>(ev->GetBufferName());
}

void DrawPrimitiveTestApp::OnIndexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<IndexBufferBuilt> ev = std::dynamic_pointer_cast<IndexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != IndexBufferName) return;
    m_idxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IIndexBufferPtr>(ev->GetBufferName());
}
