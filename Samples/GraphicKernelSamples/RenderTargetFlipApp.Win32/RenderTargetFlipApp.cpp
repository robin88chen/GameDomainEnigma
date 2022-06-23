#include "RenderTargetFlipApp.h"
#include "Controllers/InstallingPolicies.h"
#include <cassert>
#include <Platforms/MemoryAllocMacro.h>

using namespace Enigma::Controllers;
using namespace Enigma::Graphics;
using namespace Enigma::Application;

RenderTargetFlipApp::RenderTargetFlipApp(const std::string app_name) : AppDelegate(app_name)
{
}

void RenderTargetFlipApp::InstallEngine()
{
    assert(m_graphicMain);
    DeviceCreatingPolicy* creating_policy = menew DeviceCreatingPolicy(IGraphicAPI::Instance(), DeviceRequiredBits(), m_asyncType, m_hwnd);
    InstallingDefaultRendererPolicy* policy = menew InstallingDefaultRendererPolicy(std::unique_ptr<DeviceCreatingPolicy>(creating_policy),
        "default_renderer", "primary_target");
    m_graphicMain->InstallRenderEngine(std::unique_ptr<InstallingPolicy>(policy));
}

void RenderTargetFlipApp::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}
