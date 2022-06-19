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
}

#endif // INSTALLING_POLICIES_H
