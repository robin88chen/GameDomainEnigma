#include "CreateDeviceApp_Jni.h"
#include "Controllers/InstallingPolicies.h"
#include <cassert>
#include <Platforms/MemoryAllocMacro.h>

using namespace Enigma::Controllers;
using namespace Enigma::Graphics;
using namespace Enigma::Application;

CreateDeviceApp::CreateDeviceApp(const std::string app_name) : AppDelegate(app_name)
{
}

void CreateDeviceApp::InstallEngine()
{
    assert(m_graphicMain);
    DeviceCreatingPolicy* policy = menew DeviceCreatingPolicy(DeviceRequiredBits(), nullptr);
    m_graphicMain->InstallRenderEngine(std::unique_ptr<InstallingPolicy>(policy));
    //medelete policy;
}

void CreateDeviceApp::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}
