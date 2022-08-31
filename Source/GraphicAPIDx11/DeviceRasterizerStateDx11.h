/*********************************************************************
 * \file   DeviceRasterizerStateDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_RASTERIZER_STATE_DX11_H
#define DEVICE_RASTERIZER_STATE_DX11_H

#include "GraphicKernel/IDeviceRasterizerState.h"
#include <d3d11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceRasterizerStateDx11 : public Graphics::IDeviceRasterizerState
    {
    public:
        DeviceRasterizerStateDx11(const std::string& name);
        DeviceRasterizerStateDx11(const DeviceRasterizerStateDx11&) = delete;
        DeviceRasterizerStateDx11(DeviceRasterizerStateDx11&&) = delete;
        virtual ~DeviceRasterizerStateDx11();
        DeviceRasterizerStateDx11& operator=(const DeviceRasterizerStateDx11&) = delete;
        DeviceRasterizerStateDx11& operator=(DeviceRasterizerStateDx11&&) = delete;

    protected:
        virtual error CreateFromData(const RasterizerStateData& data) override;
        virtual error BindToDevice() override;

    private:
        D3D11_FILL_MODE FillModeByData(RasterizerStateData::FillMode mode);
        D3D11_CULL_MODE CullModeByData(RasterizerStateData::BackfaceCullMode mode);
        BOOL BackfaceWiseByData(RasterizerStateData::BackfaceCullMode mode);
        int PixeledDepthBiasByData(float bias);  ///< dx11 depth bias 是整數，轉換方式與depth format 有關
    private:
        ID3D11RasterizerState* m_d3dRasterizer;
    };
}


#endif // DEVICE_RASTERIZER_STATE_DX11_H
