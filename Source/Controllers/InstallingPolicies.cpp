#include "InstallingPolicies.h"
#include "ControllerEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::Controllers;

using error = std::error_code;

DeviceCreatingPolicy::DeviceCreatingPolicy(
    const Graphics::DeviceRequiredBits& rqb, void* hwnd) : InstallingPolicy()
{
    m_rqb = rqb;
    m_hwnd = hwnd;
}

DefaultRendererInstallingPolicy::DefaultRendererInstallingPolicy(
    const std::string& renderer_name, const std::string& primary_target_name) : RendererInstallingPolicy()
{
    m_rendererName = renderer_name;
    m_primaryTargetName = primary_target_name;
}

error DefaultRendererInstallingPolicy::WhenInstalling(const std::shared_ptr<Renderer::RendererManager>& manager)
{
    assert(manager);
    error er = manager->CreateRenderer(m_rendererName);
    if (er) return er;
    er = manager->CreateRenderTarget(m_primaryTargetName, Renderer::RenderTarget::PrimaryType::IsPrimary);
    return er;
}

error DefaultRendererInstallingPolicy::WhenShutdown(const std::shared_ptr<Renderer::RendererManager>& manager)
{
    assert(manager);
    error er = manager->DestroyRenderer(m_rendererName);
    if (er) return er;
    er = manager->DestroyRenderTarget(m_primaryTargetName);
    return er;
}

SceneGraphBuildingPolicy::SceneGraphBuildingPolicy(
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer) : InstallingPolicy()
{
    m_dtoDeserializer = dto_deserializer;
    m_effectDeserializer = effect_deserializer;
}

InputHandlerInstallingPolicy::InputHandlerInstallingPolicy(const ServiceCreator& creator) : InstallingPolicy()
{
    m_creator = creator;
}

std::shared_ptr<Enigma::Frameworks::ISystemService> InputHandlerInstallingPolicy::CreateService(Frameworks::ServiceManager* manager)
{
    m_inputHandler = std::shared_ptr<InputHandlers::InputHandlerService>(dynamic_cast<InputHandlers::InputHandlerService*>(m_creator(manager)));
    return m_inputHandler;
}

std::shared_ptr<DeviceCreatingPolicy> InstallingPolicyGroup::FindDeviceCreatingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto& policy : m_policies)
    {
        if (auto p = std::dynamic_pointer_cast<DeviceCreatingPolicy, InstallingPolicy>(policy)) return p;
    }
    return nullptr;
}

std::shared_ptr<RendererInstallingPolicy> InstallingPolicyGroup::FindRendererInstallingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto& policy : m_policies)
    {
        if (auto p = std::dynamic_pointer_cast<RendererInstallingPolicy, InstallingPolicy>(policy)) return p;
    }
    return nullptr;
}

std::shared_ptr<SceneGraphBuildingPolicy> InstallingPolicyGroup::FindSceneGraphBuildingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto& policy : m_policies)
    {
        if (auto p = std::dynamic_pointer_cast<SceneGraphBuildingPolicy, InstallingPolicy>(policy)) return p;
    }
    return nullptr;
}

std::shared_ptr<InputHandlerInstallingPolicy> InstallingPolicyGroup::FindInputHandlerInstallingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto& policy : m_policies)
    {
        if (auto p = std::dynamic_pointer_cast<InputHandlerInstallingPolicy, InstallingPolicy>(policy)) return p;
    }
    return nullptr;
}
