#include "RenderTargetFlipApp.h"
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
Enigma::MathLib::ColorRGBA bb_color[5] = {
    Enigma::MathLib::ColorRGBA(0.5f, 0.5f, 0.5f, 0.0f),
    Enigma::MathLib::ColorRGBA(1.0f, 0.0f, 0.5f, 0.0f),
    Enigma::MathLib::ColorRGBA(0.0f, 1.0f, 0.5f, 0.0f),
    Enigma::MathLib::ColorRGBA(0.0f, 0.0f, 0.5f, 0.0f),
    Enigma::MathLib::ColorRGBA(1.0f, 0.5f, 0.0f, 0.0f) };
static int bb_color_idx = 0;
static int time_tick = 0;

std::string PrimaryTargetName = "primary_target";

RenderTargetFlipApp::RenderTargetFlipApp(const std::string app_name) : AppDelegate(app_name)
{
}

RenderTargetFlipApp::~RenderTargetFlipApp()
{
}

void RenderTargetFlipApp::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    assert(m_graphicMain);
    DeviceCreatingPolicy* creating_policy = menew DeviceCreatingPolicy(IGraphicAPI::Instance(), DeviceRequiredBits(), m_asyncType, m_hwnd);
    InstallingDefaultRendererPolicy* policy = menew InstallingDefaultRendererPolicy(std::unique_ptr<DeviceCreatingPolicy>(creating_policy),
        "default_renderer", PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::unique_ptr<InstallingPolicy>(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();
}

void RenderTargetFlipApp::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
}

void RenderTargetFlipApp::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    time_tick++;
    if (time_tick > 500)
    {
        time_tick = 0;
        bb_color_idx++;
        if (bb_color_idx >= 5) bb_color_idx = 0;
        CommandBus::Post(ICommandPtr{ menew ChangeTargetClearingProperty{PrimaryTargetName, bb_color[bb_color_idx], std::nullopt, std::nullopt, std::nullopt} });
    }
}

void RenderTargetFlipApp::RenderFrame()
{
    if (!m_renderTarget.expired())
    {
        m_renderTarget.lock()->AsyncClear();
        m_renderTarget.lock()->AsyncFlip();
    }
}

void RenderTargetFlipApp::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    PrimaryRenderTargetCreated* ev = dynamic_cast<PrimaryRenderTargetCreated*>(e.get());
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}
