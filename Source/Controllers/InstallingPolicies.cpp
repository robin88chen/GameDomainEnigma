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
