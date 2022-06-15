#include "GraphicMain.h"
#include "EventPublisher.h"
#include "CommandBus.h"
#include "MemoryAllocMacro.h"
#include "ControllerErrors.h"
#include "ControllerEvents.h"
#include "InstallingPolicies.h"
#include <cassert>

using namespace Enigma::Controllers;
GraphicMain* GraphicMain::m_instance = nullptr;

GraphicMain::GraphicMain(GraphicCoordSys coordSys)
{
    assert(!m_instance);
#if TARGET_PLATFORM == PLATFORM_ANDROID
    m_asyncType = Graphics::IGraphicAPI::AsyncType::NotAsyncDevice;  // android always use sync device
#else
    m_asyncType = Graphics::IGraphicAPI::AsyncType::UseAsyncDevice;
#endif
    m_coordSys = coordSys;
    m_instance = this;
    m_serviceManager = menew Frameworks::ServiceManager();
}

GraphicMain::~GraphicMain()
{
    m_instance = nullptr;
    delete m_serviceManager;
}

GraphicMain* GraphicMain::Instance()
{
    assert(m_instance);
    return m_instance;
}

error GraphicMain::InstallFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->RegisterSystemService(menew Frameworks::EventPublisher(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Frameworks::CommandBus(m_serviceManager));

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew FrameworksInstalled });

    return ErrorCode::ok;
}

error GraphicMain::ShutdownFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->ShutdownSystemService(Frameworks::EventPublisher::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::CommandBus::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallRenderEngine(InstallingPolicy* policy)
{
    assert(policy);
    error er = ErrorCode::unknownInstallingPolicy;
    if (typeid(policy) == typeid(DeviceCreatingPolicy*))
    {
        er = CreateRenderEngineDevice(dynamic_cast<DeviceCreatingPolicy*>(policy));
    }
    return er;
}

error GraphicMain::ShutdownRenderEngine()
{
    error er;
    er = CleanupRenderEngineDevice();
    return er;
}

void GraphicMain::FrameUpdate()
{
    if (m_serviceManager)
    {
        m_serviceManager->RunOnce();
    }
}

error GraphicMain::CreateRenderEngineDevice(DeviceCreatingPolicy* policy)
{
    assert(policy);

#if TARGET_PLATFORM != PLATFORM_ANDROID
    m_asyncType = policy->AsyncType();
#endif
    
    if (m_asyncType == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
    {
        return policy->GraphicAPI()->AsyncCreateDevice(policy->RequiredBits(), policy->Hwnd()).get();
    }
    else
    {
        return policy->GraphicAPI()->CreateDevice(policy->RequiredBits(), policy->Hwnd());
    }
}

error GraphicMain::CleanupRenderEngineDevice()
{
    assert(Graphics::IGraphicAPI::Instance());
    if (m_asyncType == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
    {
        return Graphics::IGraphicAPI::Instance()->AsyncCleanupDevice().get();
    }
    else
    {
        return Graphics::IGraphicAPI::Instance()->CleanupDevice();
    }
}
