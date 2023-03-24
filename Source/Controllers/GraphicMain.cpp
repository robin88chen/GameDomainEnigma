#include "GraphicMain.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Renderer/RendererManager.h"
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
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::EventPublisher>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared <Frameworks::CommandBus>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::RequestBus>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::ResponseBus>(m_serviceManager));

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

    er = CreateRenderEngineDevice(m_policies.FindDeviceCreatingPolicy());
    if (er) return er;

    // 底下是目前還沒有使用 policy 的部分
    //{
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::GenericDtoFactories>(m_serviceManager));
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
    er = InstallRenderManagers();
    if (er) return er;
    //}

    er = InstallRenderer(m_policies.FindRendererInstallingPolicy());
    if (er) return er;
    er = InstallSceneGraphManagers(m_policies.FindSceneGraphBuildingPolicy());

    return er;
}

error GraphicMain::ShutdownRenderEngine()
{
    error er;
    er = ShutdownSceneGraphManagers();
    er = ShutdownRenderer(m_policies.FindRendererInstallingPolicy());

    // 底下是目前還沒有使用 policy 的部分
    //{
    er = ShutdownRenderManagers();
    er = ShutdownAnimationServices();
    er = ShutdownRenderBufferManagers();
    er = ShutdownTextureManagers();
    er = ShutdownShaderManagers();
    er = ShutdownGeometryManagers();
    m_serviceManager->ShutdownSystemService(Engine::GenericDtoFactories::TYPE_RTTI);
    //}

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

error GraphicMain::InstallRenderer(const std::shared_ptr<RendererInstallingPolicy>& policy)
{
    if (!policy) return ErrorCode::ok;
    return policy->WhenInstalling(m_serviceManager->GetSystemServiceAs<Renderer::RendererManager>());
}

error GraphicMain::ShutdownRenderer(const std::shared_ptr<RendererInstallingPolicy>& policy)
{
    if (!policy) return ErrorCode::ok;
    return policy->WhenShutdown(m_serviceManager->GetSystemServiceAs<Renderer::RendererManager>());
}

error GraphicMain::InstallRenderManagers()
{
    auto renderer = std::make_shared<Renderer::RendererManager>(m_serviceManager);
    m_serviceManager->RegisterSystemService(renderer);
    m_serviceManager->RegisterSystemService(std::make_shared<Renderer::RenderablePrimitiveBuilder>(m_serviceManager));

    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderManagers()
{
    m_serviceManager->ShutdownSystemService(Renderer::RenderablePrimitiveBuilder::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Renderer::RendererManager::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallGeometryManagers()
{
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::GeometryRepository>(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownGeometryManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::GeometryRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallShaderManagers()
{
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::ShaderRepository>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::EffectMaterialManager>(m_serviceManager));
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
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::RenderBufferRepository>(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderBufferManagers()
{
    m_serviceManager->ShutdownSystemService(Engine::RenderBufferRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallTextureManagers()
{
    m_serviceManager->RegisterSystemService(std::make_shared<Engine::TextureRepository>(m_serviceManager));
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
    m_serviceManager->RegisterSystemService(std::make_shared<SceneGraph::SceneGraphRepository>(m_serviceManager, policy->GetDtoDeserializer(),
        policy->GetEffectDeserializer()));
    m_serviceManager->RegisterSystemService(std::make_shared<SceneGraph::LazyNodeIOService>(m_serviceManager, timer, policy->GetDtoDeserializer()));
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
    auto timer = std::make_shared<Engine::TimerService>(m_serviceManager);
    m_serviceManager->RegisterSystemService(timer);
    m_serviceManager->RegisterSystemService(std::make_shared<Animators::AnimationRepository>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Animators::AnimationFrameListener>(m_serviceManager, timer));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownAnimationServices()
{
    m_serviceManager->ShutdownSystemService(Animators::AnimationFrameListener::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Animators::AnimationRepository::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Engine::TimerService::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallInputHandlers(const std::shared_ptr<InputHandlerInstallingPolicy>& policy)
{
    assert(policy);
    m_serviceManager->RegisterSystemService(policy->CreateService(m_serviceManager));
    return ErrorCode::ok;
}

error GraphicMain::ShutdownInputHandlers()
{
    m_serviceManager->ShutdownSystemService(InputHandlers::InputHandlerService::TYPE_RTTI);
    return ErrorCode::ok;
}
