/*********************************************************************
 * \file   DeviceRasterizerStateEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_RASTERIZER_STATE_EGL_H
#define DEVICE_RASTERIZER_STATE_EGL_H

#include "GraphicKernel/IDeviceRasterizerState.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceRasterizerStateEgl : public Graphics::IDeviceRasterizerState
    {
    public:
        DeviceRasterizerStateEgl(const std::string& name);
        DeviceRasterizerStateEgl(const DeviceRasterizerStateEgl&) = delete;
        DeviceRasterizerStateEgl(DeviceRasterizerStateEgl&&) = delete;
        virtual ~DeviceRasterizerStateEgl();
        DeviceRasterizerStateEgl& operator=(const DeviceRasterizerStateEgl&) = delete;
        DeviceRasterizerStateEgl& operator=(DeviceRasterizerStateEgl&&) = delete;

        virtual error CreateFromData(const RasterizerStateData& data) override;

        virtual error BindToDevice() override;

    private:
        bool IsCullEnableByData(RasterizerStateData::BackfaceCullMode mode);
        GLenum FrontFaceWiseByData(RasterizerStateData::BackfaceCullMode mode);
        float PixeledDepthBiasByData(float bias) const;  ///< 轉換方式與depth format 有關
    private:
        bool m_isCullEnable;
        GLenum m_cullMode;
        GLenum m_frontFaceWise;
        float m_depthSlopeFactor;
        float m_depthBias;
    };
}


#endif // DEVICE_RASTERIZER_STATE_EGL_H
