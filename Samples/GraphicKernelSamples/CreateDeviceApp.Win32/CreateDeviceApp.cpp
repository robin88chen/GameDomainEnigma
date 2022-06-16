#include "CreateDeviceApp.h"
#include "InstallingPolicies.h"
#include <cassert>
#include <MemoryAllocMacro.h>

using namespace Enigma::Controllers;
using namespace Enigma::Graphics;
using namespace Enigma::Application;

CreateDeviceApp::CreateDeviceApp(const std::string app_name) : AppDelegate(app_name)
{
}

void CreateDeviceApp::InstallEngine()
{
    assert(m_graphicMain);
    DeviceCreatingPolicy* policy = menew DeviceCreatingPolicy(IGraphicAPI::Instance(), DeviceRequiredBits(), m_asyncType, m_hwnd);
    m_graphicMain->InstallRenderEngine(policy);
    medelete policy;
}

void CreateDeviceApp::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}
