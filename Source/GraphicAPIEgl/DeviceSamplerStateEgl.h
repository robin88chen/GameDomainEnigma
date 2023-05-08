/*********************************************************************
 * \file   DeviceSamplerStateEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_SAMPLER_STATE_EGL_H
#define DEVICE_SAMPLER_STATE_EGL_H

#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/ExtentTypesDefine.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceSamplerStateEgl : public Graphics::IDeviceSamplerState
    {
    public:
        DeviceSamplerStateEgl(const std::string& name);
        DeviceSamplerStateEgl(const DeviceSamplerStateEgl&) = delete;
        DeviceSamplerStateEgl(DeviceSamplerStateEgl&&) = delete;
        virtual ~DeviceSamplerStateEgl() override;
        DeviceSamplerStateEgl& operator=(const DeviceSamplerStateEgl&) = delete;
        DeviceSamplerStateEgl& operator=(DeviceSamplerStateEgl&&) = delete;

        virtual error CreateFromData(const SamplerStateData& data) override;

        virtual error BindToShader(unsigned int bindPoint);
        virtual future_error AsyncBindToShader(unsigned int bindPoint);

    private:
        GLint AddressModeByData(SamplerStateData::AddressMode mode);
        GLint MinFilterByData(SamplerStateData::Filter min_fil, SamplerStateData::Filter mip_fil);
        GLint MagFilterByData(SamplerStateData::Filter fil);

    private:
        GLuint m_sampler;
    };
}


#endif // DEVICE_SAMPLER_STATE_EGL_H
