#include "InstallingPolicies.h"

Enigma::Controllers::DeviceCreatingPolicy::DeviceCreatingPolicy(
    Graphics::IGraphicAPI* api, const Graphics::DeviceRequiredBits& rqb, void* hwnd) : InstallingPolicy()
{
    m_api = api;
    m_rqb = rqb;
    m_hwnd = hwnd;
}

Enigma::Controllers::InstallingDefaultRendererPolicy::InstallingDefaultRendererPolicy(
    std::unique_ptr<DeviceCreatingPolicy> creating_policy, const std::string& renderer_name,
    const std::string& primary_target_name) : InstallingPolicy()
{
    m_creatingPolicy = std::move(creating_policy);
    m_rendererName = renderer_name;
    m_primaryTargetName = primary_target_name;
}
