#include "InstallingPolicies.h"

Enigma::Controllers::DeviceCreatingPolicy::DeviceCreatingPolicy(
    const Graphics::DeviceRequiredBits& rqb, void* hwnd) : InstallingPolicy()
{
    m_rqb = rqb;
    m_hwnd = hwnd;
}

Enigma::Controllers::InstallingDefaultRendererPolicy::InstallingDefaultRendererPolicy(
    const std::string& renderer_name, const std::string& primary_target_name) : InstallingPolicy()
{
    m_rendererName = renderer_name;
    m_primaryTargetName = primary_target_name;
}

Enigma::Controllers::SceneGraphBuildingPolicy::SceneGraphBuildingPolicy(
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer) : InstallingPolicy()
{
    m_dtoDeserializer = dto_deserializer;
    m_effectDeserializer = effect_deserializer;
}
