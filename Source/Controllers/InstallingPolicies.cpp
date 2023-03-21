#include "InstallingPolicies.h"

using namespace Enigma::Controllers;

DeviceCreatingPolicy::DeviceCreatingPolicy(
    const Graphics::DeviceRequiredBits& rqb, void* hwnd) : InstallingPolicy()
{
    m_rqb = rqb;
    m_hwnd = hwnd;
}

InstallingDefaultRendererPolicy::InstallingDefaultRendererPolicy(
    const std::string& renderer_name, const std::string& primary_target_name) : InstallingPolicy()
{
    m_rendererName = renderer_name;
    m_primaryTargetName = primary_target_name;
}

SceneGraphBuildingPolicy::SceneGraphBuildingPolicy(
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer) : InstallingPolicy()
{
    m_dtoDeserializer = dto_deserializer;
    m_effectDeserializer = effect_deserializer;
}

InstallingInputHandlerPolicy::InstallingInputHandlerPolicy(const ServiceCreator& creator) : InstallingPolicy()
{
    m_creator = creator;
}

std::shared_ptr<Enigma::Frameworks::ISystemService> InstallingInputHandlerPolicy::CreateService(Frameworks::ServiceManager* manager)
{
    m_inputHandler = std::shared_ptr<InputHandlers::InputHandlerService>(dynamic_cast<InputHandlers::InputHandlerService*>(m_creator(manager)));
    return m_inputHandler;
}

std::shared_ptr<DeviceCreatingPolicy> InstallingPolicyGroup::FindDeviceCreatingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(DeviceCreatingPolicy)) return std::dynamic_pointer_cast<DeviceCreatingPolicy, InstallingPolicy>(policy);
    }
    return nullptr;
}

std::shared_ptr<InstallingDefaultRendererPolicy> InstallingPolicyGroup::FindRendererInstallingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(InstallingDefaultRendererPolicy)) return std::dynamic_pointer_cast<InstallingDefaultRendererPolicy, InstallingPolicy>(policy);
    }
    return nullptr;
}

std::shared_ptr<SceneGraphBuildingPolicy> InstallingPolicyGroup::FindSceneGraphBuildingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(SceneGraphBuildingPolicy)) return std::dynamic_pointer_cast<SceneGraphBuildingPolicy, InstallingPolicy>(policy);
    }
    return nullptr;
}

std::shared_ptr<InstallingInputHandlerPolicy> InstallingPolicyGroup::FindInputHandlerInstallingPolicy()
{
    if (m_policies.empty()) return nullptr;
    for (auto policy : m_policies)
    {
        auto& p = *policy;
        if (typeid(p) == typeid(InstallingInputHandlerPolicy)) return std::dynamic_pointer_cast<InstallingInputHandlerPolicy, InstallingPolicy>(policy);
    }
    return nullptr;
}
