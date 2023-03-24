/*********************************************************************
 * \file   DeviceCreatingPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _DEVICE_CREATING_POLICY_H
#define _DEVICE_CREATING_POLICY_H

#include "InstallingPolicy.h"
#include "GraphicKernel/DeviceRequiredBits.h"

namespace Enigma::Engine
{
    class DeviceCreatingPolicy : public InstallingPolicy
    {
    public:
        DeviceCreatingPolicy(const Graphics::DeviceRequiredBits& rqb, void* hwnd) : m_rqb(rqb), m_hwnd(hwnd) {}
        virtual ~DeviceCreatingPolicy() override = default;

        virtual void Install(Frameworks::ServiceManager* service_manager) override;
        virtual void Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        Graphics::DeviceRequiredBits m_rqb;
        void* m_hwnd;
    };
}

#endif // _DEVICE_CREATING_POLICY_H
