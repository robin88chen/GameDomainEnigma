#include "GraphicMain.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Renderer/RendererManager.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/RenderablePrimitiveBuilder.h"
#include "GameEngine/ShaderRepository.h"
#include "GameEngine/EffectMaterialManager.h"
#include "GameEngine/MaterialVariableMap.h"
#include "GameEngine/RenderBufferRepository.h"
#include "GameEngine/TextureRepository.h"
#include "GameEngine/GenericDtoFactories.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/LazyNodeIOService.h"
#include "GameEngine/GeometryRepository.h"
#include "GameEngine/TimerService.h"
#include "Animators/AnimationRepository.h"
#include "Animators/AnimationFrameListener.h"
#include "ControllerErrors.h"
#include "ControllerEvents.h"
#include "InstallingPolicies.h"
#include <cassert>
#include <memory>

using namespace Enigma::Controllers;
GraphicMain* GraphicMain::m_instance = nullptr;

GraphicMain::GraphicMain()
{
    assert(!m_instance);
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
    m_serviceManager->RegisterSystemService(menew Frameworks::RequestBus(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Frameworks::ResponseBus(m_serviceManager));

    Frameworks::EventPublisher::Post(std::make_shared<FrameworksInstalled>());

    return ErrorCode::ok;
}

error GraphicMain::ShutdownFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->ShutdownSystemService(Frameworks::EventPublisher::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::CommandBus::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::RequestBus::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::ResponseBus::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallRenderEngine(const std::vector<std::shared_ptr<InstallingPolicy>>& policies)
{
    assert(policies.size());
    m_policies = policies;
    error er = ErrorCode::unknownInstallingPolicy;

    er = CreateRenderEngineDevice(std::dynamic_pointer_cast<DeviceCreatingPolicy, InstallingPolicy>(FindDeviceCreatingPolicy()));
    if (er) return er;
    er = InstallDefaultRenderer(std::dynamic_pointer_cast<InstallingDefaultRendererPolicy, InstallingPolicy>(FindRendererInstallingPolicy()));
    if (er) return er;
    er = InstallSceneGraphManagers(std::dynamic_pointer_cast<SceneGraphBuildingPolicy, InstallingPolicy>(FindSceneGraphBuildingPolicy()));

    return er;
}

error GraphicMain::ShutdownRenderEngine()
{
    error er;
    er = ShutdownSceneGraphManagers();
    er = ShutdownDefaultRenderer();
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

error GraphicMain::CreateRenderEngineDevice(const std::shared_ptr<DeviceCreatingPolicy>& policy)
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

error GraphicMain::InstallDefaultRenderer(const std::shared_ptr<InstallingDefaultRendererPolicy>& policy)
{
    error er;

    m_serviceManager->RegisterSystemService(menew Engine::GenericDtoFactories(m_serviceManager));

    er = InstallGeometryManagers();
    if (er) return er;
    er = InstallShaderManagers();
    if (er) return er;
    er = InstallTextureManagers();
    if (er) return er;
    er = InstallRenderBufferManagers();
    if (er) return er;

    er = InstallAnimationServices();
    if (er) return er;
    er = InstallRenderer(policy->GetRendererName(), policy->GetPrimaryTargetName(), true);
    return er;
}

error GraphicMain::ShutdownDefaultRenderer()
{
    std::shared_ptr<InstallingDefaultRendererPolicy> policy = std::dynamic_pointer_cast<InstallingDefaultRendererPolicy, InstallingPolicy>(
        FindRendererInstallingPolicy());
    assert(policy);

    error er;
    er = ShutdownRenderer(policy->GetRendererName(), policy->GetPrimaryTargetName());
    er = ShutdownAnimationServices();

    er = ShutdownRenderBufferManagers();
    er = ShutdownTextureManagers();
    er = ShutdownShaderManagers();
    er = ShutdownGeometryManagers();
    if (er) return er;

    m_serviceManager->ShutdownSystemService(Engine::GenericDtoFactories::TYPE_RTTI);

    return er;
}

error GraphicMain::InstallRenderer(const std::string& renderer_name, const std::string render_target_name, bool is_primary)
{
    m_renderer = menew Renderer::RendererManager(m_serviceManager);
    m_serviceManager->RegisterSystemService(m_renderer);
    error er = m_renderer->CreateRenderer(renderer_name);
    if (er) return er;
    er = m_renderer->CreateRenderTarget(render_target_name,
        is_primary ? Renderer::RenderTarget::PrimaryType::IsPrimary : Renderer::RenderTarget::PrimaryType::NotPrimary);
    if (er) return er;

    m_serviceManager->RegisterSystemService(menew Renderer::RenderablePrimitiveBuilder(m_serviceManager));
    Frameworks::EventPublisher::Post(std::make_shared<DefaultRendererInstalled>());

    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderer(const std::string& renderer_name, const std::string render_target_name)
{
    m_serviceManager->ShutdownSystemService(Renderer::RenderablePrimitiveBuilder::TYPE_RTTI);

    if (m_renderer)
    {
        error er = m_renderer->DestroyRenderer(renderer_name);
        if (er) return er;
        er = m_renderer->DestroyRenderTarget(render_target_name);
        if (er) return er;
    }
    m_serviceManager->ShutdownSystemService(Renderer::RendererManager::TYPE_RTTI);

    return ErrorCode::ok;
}

error GraphicMain::InstallGeometryManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::GeometryRepository(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownGeometryManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::GeometryRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallShaderManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::ShaderRepository(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Engine::EffectMaterialManager(m_serviceManager));
    menew Engine::MaterialVariableMap;
    return ErrorCode::ok;
}

error GraphicMain::ShutdownShaderManagers()
{
    delete Engine::MaterialVariableMap::Instance();
    m_serviceManager->ShutdownSystemService(Engine::EffectMaterialManager::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Engine::ShaderRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallRenderBufferManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::RenderBufferRepository(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderBufferManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::RenderBufferRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallTextureManagers()
{
    m_serviceManager->RegisterSystemService(menew Engine::TextureRepository(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownTextureManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::TextureRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallSceneGraphManagers(const std::shared_ptr<SceneGraphBuildingPolicy>& policy)
{
    assert(policy);
    const auto timer = m_serviceManager->GetSystemServiceAs<Engine::TimerService>();
    assert(timer);
    m_serviceManager->RegisterSystemService(menew SceneGraph::SceneGraphRepository(m_serviceManager, policy->GetDtoDeserializer(),
        policy->GetEffectDeserializer()));
    m_serviceManager->RegisterSystemService(menew SceneGraph::LazyNodeIOService(m_serviceManager, timer, policy->GetDtoDeserializer()));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownSceneGraphManagers()
{
    m_serviceManager->ShutdownSystemService(SceneGraph::LazyNodeIOService::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(SceneGraph::SceneGraphRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallAnimationServices()
{
    //todo : timer 跟 animation 先放在一起，以後有適合的地方再改
    auto timer = menew Engine::TimerService(m_serviceManager);
    m_serviceManager->RegisterSystemService(timer);
    m_serviceManager->RegisterSystemService(menew Animators::AnimationRepository(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Animators::AnimationFrameListener(m_serviceManager, timer));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownAnimationServices()
{
    m_serviceManager->ShutdownSystemService(Animators::AnimationFrameListener::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Animators::AnimationRepository::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Engine::TimerService::TYPE_RTTI);
    return ErrorCode::ok;
}

std::shared_ptr<InstallingPolicy> GraphicMain::FindDeviceCreatingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(DeviceCreatingPolicy)) return policy;
    }
    return nullptr;
}

std::shared_ptr<InstallingPolicy> GraphicMain::FindRendererInstallingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(InstallingDefaultRendererPolicy)) return policy;
    }
    return nullptr;
}

std::shared_ptr<InstallingPolicy> GraphicMain::FindSceneGraphBuildingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(SceneGraphBuildingPolicy)) return policy;
    }
    return nullptr;
}
