#include "GraphicMain.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "GameEngine/RendererManager.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/ShaderManager.h"
#include "GameEngine/EffectMaterialManager.h"
#include "GameEngine/RenderBufferManager.h"
#include "ControllerErrors.h"
#include "ControllerEvents.h"
#include "InstallingPolicies.h"
#include <cassert>
#include <memory>

using namespace Enigma::Controllers;
GraphicMain* GraphicMain::m_instance = nullptr;

GraphicMain::GraphicMain(GraphicCoordSys coordSys)
{
    assert(!m_instance);
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

    Frameworks::EventPublisher::Post(std::make_shared<FrameworksInstalled>());

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

    Frameworks::CommandBus::Send(std::make_shared<Graphics::CreateDevice>(policy->RequiredBits(), policy->Hwnd()));
    return ErrorCode::ok;
}

error GraphicMain::CleanupRenderEngineDevice()
{
    Frameworks::CommandBus::Send(std::make_shared<Graphics::CleanupDevice>());
    return ErrorCode::ok;
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
    if (er) return er;
    er = InstallShaderManagers();
    if (er) return er;
    er = InstallRenderBufferManagers();
    return er;
}

error GraphicMain::ShutdownDefaultRenderer()
{
    InstallingDefaultRendererPolicy* policy = dynamic_cast<InstallingDefaultRendererPolicy*>(m_policy.get());
    assert(policy);

    error er;
    er = ShutdownRenderBufferManagers();
    er = ShutdownShaderManagers();
    er = ShutdownRenderer(policy->GetRendererName(), policy->GetPrimaryTargetName());
    if (er) return er;

    if (policy->GetDeviceCreatingPolicy())
    {
        er = CleanupRenderEngineDevice();
        if (er) return er;
    }
    return er;
}

error GraphicMain::InstallRenderer(const std::string& renderer_name, const std::string render_target_name, bool is_primary)
{
    m_renderer = menew Engine::RendererManager(m_serviceManager);
    m_serviceManager->RegisterSystemService(m_renderer);
    error er = m_renderer->CreateRenderer(renderer_name);
    if (er) return er;
    er = m_renderer->CreateRenderTarget(render_target_name,
        is_primary ? Engine::RenderTarget::PrimaryType::IsPrimary : Engine::RenderTarget::PrimaryType::NotPrimary);
    if (er) return er;

    Frameworks::EventPublisher::Post(std::make_shared<DefaultRendererInstalled>());

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

error GraphicMain::InstallShaderManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::ShaderManager(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Engine::EffectMaterialManager(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownShaderManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::EffectMaterialManager::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Engine::ShaderManager::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallRenderBufferManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::RenderBufferManager(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderBufferManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::RenderBufferManager::TYPE_RTTI);
    return ErrorCode::ok;
}
