/*********************************************************************
 * \file   InstallingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef INSTALLING_POLICIES_H
#define INSTALLING_POLICIES_H

#include "GraphicKernel/IGraphicAPI.h"
#include <memory>

namespace Enigma::Controllers
{
    class InstallingPolicy
    {
    public:
        InstallingPolicy() = default;
        virtual ~InstallingPolicy() = default;
    };
    class DeviceCreatingPolicy : public InstallingPolicy
    {
    public:
        DeviceCreatingPolicy(Graphics::IGraphicAPI* api, const Graphics::DeviceRequiredBits& rqb,
            Graphics::IGraphicAPI::AsyncType async, void* hwnd);

        Graphics::IGraphicAPI* GraphicAPI() { return m_api; }
        const Graphics::DeviceRequiredBits& RequiredBits() { return m_rqb; }
        Graphics::IGraphicAPI::AsyncType AsyncType() { return m_async; }
        void* Hwnd() { return m_hwnd; }
    protected:
        Graphics::IGraphicAPI* m_api;
        Graphics::DeviceRequiredBits m_rqb;
        Graphics::IGraphicAPI::AsyncType m_async;
        void* m_hwnd;
    };
    class InstallingDefaultRendererPolicy : public InstallingPolicy
    {
    public:
        InstallingDefaultRendererPolicy(std::unique_ptr<DeviceCreatingPolicy> creating_policy, const std::string& renderer_name,
            const std::string& primary_target_name);

        DeviceCreatingPolicy* GetDeviceCreatingPolicy() { return m_creatingPolicy.get(); }
        const std::string& GetRendererName() { return m_rendererName; }
        const std::string& GetPrimaryTargetName() { return m_primaryTargetName; }

    protected:
        std::unique_ptr<DeviceCreatingPolicy> m_creatingPolicy;
        std::string m_rendererName;
        std::string m_primaryTargetName;
    };
}

#endif // INSTALLING_POLICIES_H
