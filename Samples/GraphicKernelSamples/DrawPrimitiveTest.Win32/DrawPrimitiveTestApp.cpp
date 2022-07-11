#include "DrawPrimitiveTestApp.h"
#include "MathLib/Vector3.h"
#include "Controllers/InstallingPolicies.h"
#include <cassert>
#include <Platforms/MemoryAllocMacro.h>
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ServiceManager.h"
#include "GameEngine/RendererEvents.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/RendererCommands.h"

using namespace Enigma::Controllers;
using namespace Enigma::Graphics;
using namespace Enigma::Application;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

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
    assert(m_graphicMain);
    DeviceCreatingPolicy* creating_policy = menew DeviceCreatingPolicy(IGraphicAPI::Instance(), DeviceRequiredBits(), m_asyncType, m_hwnd);
    InstallingDefaultRendererPolicy* policy = menew InstallingDefaultRendererPolicy(std::unique_ptr<DeviceCreatingPolicy>(creating_policy),
        DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::unique_ptr<InstallingPolicy>(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();
}

void DrawPrimitiveTestApp::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
}

void DrawPrimitiveTestApp::FrameUpdate()
{
    AppDelegate::FrameUpdate();
}

void DrawPrimitiveTestApp::RenderFrame()
{
    AppDelegate::RenderFrame();
}

void DrawPrimitiveTestApp::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    PrimaryRenderTargetCreated* ev = dynamic_cast<PrimaryRenderTargetCreated*>(e.get());
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}
