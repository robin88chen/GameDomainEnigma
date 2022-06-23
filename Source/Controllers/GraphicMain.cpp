#include "GraphicMain.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "GameEngine/RendererManager.h"
#include "GameEngine/RenderTarget.h"
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
    m_renderer = nullptr;
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

error GraphicMain::InstallRenderEngine(std::unique_ptr<InstallingPolicy> policy)
{
    assert(policy);
    m_policy = std::move(policy);
    error er = ErrorCode::unknownInstallingPolicy;
    auto& p = *m_policy;
    if (typeid(p) == typeid(DeviceCreatingPolicy))
    {
        er = CreateRenderEngineDevice(dynamic_cast<DeviceCreatingPolicy*>(m_policy.get()));
    }
    else if (typeid(p) == typeid(InstallingDefaultRendererPolicy))
    {
        er = InstallDefaultRenderer(dynamic_cast<InstallingDefaultRendererPolicy*>(m_policy.get()));
    }
    return er;
}

error GraphicMain::ShutdownRenderEngine()
{
    error er;
    auto& p = *m_policy;
    if (typeid(p) == typeid(DeviceCreatingPolicy))
    {
        er = CleanupRenderEngineDevice();
    }
    else if (typeid(p) == typeid(InstallingDefaultRendererPolicy))
    {
        er = ShutdownDefaultRenderer();
    }
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

error GraphicMain::InstallDefaultRenderer(InstallingDefaultRendererPolicy* policy)
{
    error er;
    if (policy->GetDeviceCreatingPolicy())
    {
        er = CreateRenderEngineDevice(policy->GetDeviceCreatingPolicy());
        if (er) return er;
    }
    er = InstallRenderer(policy->GetRendererName(), policy->GetPrimaryTargetName(), true);
    return er;
}

error GraphicMain::ShutdownDefaultRenderer()
{
    error er;
    InstallingDefaultRendererPolicy* policy = dynamic_cast<InstallingDefaultRendererPolicy*>(m_policy.get());
    assert(policy);
    if (policy->GetDeviceCreatingPolicy())
    {
        er = CleanupRenderEngineDevice();
        if (er) return er;
    }
    er = ShutdownRenderer(policy->GetRendererName(), policy->GetPrimaryTargetName());
    return er;
}

error GraphicMain::InstallRenderer(const std::string& renderer_name, const std::string render_target_name, bool is_primary)
{
    m_renderer = menew Engine::RendererManager(m_serviceManager);
    m_serviceManager->RegisterSystemService(m_renderer);
    error er = m_renderer->CreateRenderer(renderer_name);
    if (er) return er;
    er = m_renderer->CreateRenderTarget(render_target_name,
        is_primary ? Engine::RenderTarget::PrimaryType::IsPrimary : Engine::RenderTarget::PrimaryType::NotPrimary,
        m_asyncType);
    if (er) return er;

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew DefaultRendererInstalled });

    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderer(const std::string& renderer_name, const std::string render_target_name)
{
    if (m_renderer)
    {
        error er = m_renderer->DestroyRenderer(renderer_name);
        if (er) return er;
        er = m_renderer->DestroyRenderTarget(render_target_name);
        if (er) return er;
    }
    m_serviceManager->ShutdownSystemService(Engine::RendererManager::TYPE_RTTI);

    return ErrorCode::ok;
}

