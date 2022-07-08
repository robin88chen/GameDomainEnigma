/*********************************************************************
 * \file   DeviceConstBufferDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_CONST_BUFFER_DX11_H
#define DEVICE_CONST_BUFFER_DX11_H

#include "GraphicKernel/IDeviceConstBuffer.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <d3d11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceConstBufferDx11 : public Graphics::IDeviceConstBuffer
    {
    public:
        DeviceConstBufferDx11(const std::string& name);
        DeviceConstBufferDx11(const DeviceConstBufferDx11&) = delete;
        DeviceConstBufferDx11(DeviceConstBufferDx11&&) = delete;
        virtual ~DeviceConstBufferDx11();
        DeviceConstBufferDx11& operator=(const DeviceConstBufferDx11&) = delete;
        DeviceConstBufferDx11& operator=(DeviceConstBufferDx11&&) = delete;

        virtual error CreateDataBuffer(unsigned int cb_size) override;

        virtual error Apply(const byte_buffer& data, unsigned int dataSize);
        virtual error Apply();
        virtual future_error AsyncApply();
        virtual error BindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned int bindPoint);
        virtual future_error AsyncBindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned int bindPoint);
    private:
        ID3D11Buffer* m_d3dBuffer;
    };
}


#endif // DEVICE_CONST_BUFFER_DX11_H
