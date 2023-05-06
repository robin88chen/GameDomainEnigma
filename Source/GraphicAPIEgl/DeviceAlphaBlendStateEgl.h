/*********************************************************************
 * \file   DeviceAlphaBlendStateEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_ALPHABLEND_STATE_EGL_H
#define DEVICE_ALPHABLEND_STATE_EGL_H

#include "GraphicKernel/IDeviceAlphaBlendState.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif
namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceAlphaBlendStateEgl : public Graphics::IDeviceAlphaBlendState
    {
    public:
        DeviceAlphaBlendStateEgl(const std::string& name);
        DeviceAlphaBlendStateEgl(const DeviceAlphaBlendStateEgl&) = delete;
        DeviceAlphaBlendStateEgl(DeviceAlphaBlendStateEgl&&) = delete;
        virtual ~DeviceAlphaBlendStateEgl() override;
        DeviceAlphaBlendStateEgl& operator=(const DeviceAlphaBlendStateEgl&) = delete;
        DeviceAlphaBlendStateEgl& operator=(DeviceAlphaBlendStateEgl&&) = delete;

    protected:
        virtual error CreateFromData(const BlendStateData& data) override;
        virtual error BindToDevice() override;

    private:
        bool m_isBlendEnable;
        GLenum m_srcFactor;
        GLenum m_dstFactor;
        GLenum m_blendOp;
    };
}


#endif // DEVICE_ALPHABLEND_STATE_EGL_H
