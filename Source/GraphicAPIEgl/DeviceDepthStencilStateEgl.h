/*********************************************************************
 * \file   DeviceDepthStencilStateEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_DEPTH_STENCIL_STATE_EGL_H
#define DEVICE_DEPTH_STENCIL_STATE_EGL_H

#include "GraphicKernel/IDeviceDepthStencilState.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceDepthStencilStateEgl : public Graphics::IDeviceDepthStencilState
    {
    public:
        DeviceDepthStencilStateEgl(const std::string& name);
        DeviceDepthStencilStateEgl(const DeviceDepthStencilStateEgl&) = delete;
        DeviceDepthStencilStateEgl(DeviceDepthStencilStateEgl&&) = delete;
        virtual ~DeviceDepthStencilStateEgl();
        DeviceDepthStencilStateEgl& operator=(const DeviceDepthStencilStateEgl&) = delete;
        DeviceDepthStencilStateEgl& operator=(DeviceDepthStencilStateEgl&&) = delete;

    protected:
        virtual error CreateFromData(const DepthStencilData& data) override;
        virtual error BindToDevice() override;

    private:
        GLenum ToCmpFunc(DepthStencilData::CompareFunc f);
        GLenum ToStencilOp(DepthStencilData::StencilOpCode op);

    private:
        GLenum m_depthFunc;
        GLenum m_frontFaceStencilFunc;
        GLenum m_frontFaceFailOp;
        GLenum m_frontFaceZFailOp;
        GLenum m_frontFacePassOp;
        GLenum m_backFaceStencilFunc;
        GLenum m_backFaceFailOp;
        GLenum m_backFaceZFailOp;
        GLenum m_backFacePassOp;
    };
}


#endif // DEVICE_DEPTH_STENCIL_STATE_EGL_H
