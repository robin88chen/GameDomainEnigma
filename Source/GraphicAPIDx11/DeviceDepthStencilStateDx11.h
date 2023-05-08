/*********************************************************************
 * \file   DeviceDepthStencilStateDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_DEPTH_STENCIL_STATE_DX11_H
#define DEVICE_DEPTH_STENCIL_STATE_DX11_H

#include "GraphicKernel/IDeviceDepthStencilState.h"
#include <d3d11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceDepthStencilStateDx11 : public Graphics::IDeviceDepthStencilState
    {
    public:
        DeviceDepthStencilStateDx11(const std::string& name);
        DeviceDepthStencilStateDx11(const DeviceDepthStencilStateDx11&) = delete;
        DeviceDepthStencilStateDx11(DeviceDepthStencilStateDx11&&) = delete;
        virtual ~DeviceDepthStencilStateDx11() override;
        DeviceDepthStencilStateDx11& operator=(const DeviceDepthStencilStateDx11&) = delete;
        DeviceDepthStencilStateDx11& operator=(DeviceDepthStencilStateDx11&&) = delete;

        virtual error CreateFromData(const DepthStencilData& data) override;

    protected:
        virtual error BindToDevice() override;

    private:
        D3D11_COMPARISON_FUNC CompareFuncByData(DepthStencilData::CompareFunc mode);
        void FillStencilOpData(D3D11_DEPTH_STENCILOP_DESC& desc, const DepthStencilData::StencilOpData& data);
        D3D11_STENCIL_OP OpCodeByData(DepthStencilData::StencilOpCode code);
    private:
        ID3D11DepthStencilState* m_d3dDepthStencil;
    };
}


#endif // DEVICE_DEPTH_STENCIL_STATE_DX11_H
