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
