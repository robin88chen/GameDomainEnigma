#include "InstallingPolicies.h"

Enigma::Controllers::DeviceCreatingPolicy::DeviceCreatingPolicy(
    Graphics::IGraphicAPI* api, const Graphics::DeviceRequiredBits& rqb, Graphics::IGraphicAPI::AsyncType async,
    void* hwnd) : InstallingPolicy()
{
    m_api = api;
    m_rqb = rqb;
    m_async = async;
    m_hwnd = hwnd;
}

Enigma::Controllers::InstallingDefaultRendererPolicy::InstallingDefaultRendererPolicy(
    DeviceCreatingPolicy* creating_policy, const std::string& renderer_name,
    const std::string& primary_target_name) : InstallingPolicy()
{
    m_creatingPolicy = creating_policy;
    m_rendererName = renderer_name;
    m_primaryTargetName = primary_target_name;
}
