/*********************************************************************
 * \file   DeviceAlphaBlendStateDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_ALPHA_BLEND_STATE_DX11_H
#define DEVICE_ALPHA_BLEND_STATE_DX11_H

#include "GraphicKernel/IDeviceAlphaBlendState.h"
#include <d3d11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceAlphaBlendStateDx11 : public Graphics::IDeviceAlphaBlendState
    {
    public:
        DeviceAlphaBlendStateDx11(const std::string& name);
        DeviceAlphaBlendStateDx11(const DeviceAlphaBlendStateDx11&) = delete;
        DeviceAlphaBlendStateDx11(DeviceAlphaBlendStateDx11&&) = delete;
        virtual ~DeviceAlphaBlendStateDx11() override;
        DeviceAlphaBlendStateDx11& operator=(const DeviceAlphaBlendStateDx11&) = delete;
        DeviceAlphaBlendStateDx11& operator=(DeviceAlphaBlendStateDx11&&) = delete;

        virtual error CreateFromData(const BlendStateData& data) override;

    protected:
        virtual error BindToDevice() override;

    private:
        void FillBlendOps(D3D11_RENDER_TARGET_BLEND_DESC& desc, BlendStateData::BlendType type);

    private:
        ID3D11BlendState* m_d3dBlend;
    };
}


#endif // DEVICE_ALPHA_BLEND_STATE_DX11_H
