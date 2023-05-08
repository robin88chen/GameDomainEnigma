/*********************************************************************
 * \file   DeviceSamplerStateDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_SAMPLER_STATE_DX11_H
#define DEVICE_SAMPLER_STATE_DX11_H

#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <d3d11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceSamplerStateDx11 : public Graphics::IDeviceSamplerState
    {
    public:
        DeviceSamplerStateDx11(const std::string& name);
        DeviceSamplerStateDx11(const DeviceSamplerStateDx11&) = delete;
        DeviceSamplerStateDx11(DeviceSamplerStateDx11&&) = delete;
        virtual ~DeviceSamplerStateDx11() override;
        DeviceSamplerStateDx11& operator=(const DeviceSamplerStateDx11&) = delete;
        DeviceSamplerStateDx11& operator=(DeviceSamplerStateDx11&&) = delete;

        virtual error CreateFromData(const SamplerStateData& data) override;

        virtual error BindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned int bindPoint);
        virtual future_error AsyncBindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned int bindPoint);

    private:
        D3D11_FILTER FilterByData();
        D3D11_TEXTURE_ADDRESS_MODE AddressModeByData(SamplerStateData::AddressMode mode);
        D3D11_COMPARISON_FUNC CompareFuncByData();
    private:
        ID3D11SamplerState* m_d3dSampler;
    };
}

#endif // DEVICE_SAMPLER_STATE_DX11_H
