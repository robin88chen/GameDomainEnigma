/*********************************************************************
 * \file   BackSurfaceEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef BACK_SURFACE_EGL_H
#define BACK_SURFACE_EGL_H

#include "Platforms/PlatformConfig.h"
#include "GraphicKernel/IBackSurface.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>

namespace Enigma::Devices
{
    using error = std::error_code;
    class BackSurfaceEgl : public Graphics::IBackSurface
    {
    public:
        BackSurfaceEgl(const std::string& name, const MathLib::Dimension<unsigned>& dimension, 
            const Graphics::GraphicFormat& fmt, bool primary);
        BackSurfaceEgl(const BackSurfaceEgl&) = delete;
        BackSurfaceEgl(BackSurfaceEgl&&) = delete;
        virtual ~BackSurfaceEgl() override;

        BackSurfaceEgl& operator=(const BackSurfaceEgl&) = delete;
        BackSurfaceEgl& operator=(BackSurfaceEgl&&) = delete;

        virtual error Resize(const MathLib::Dimension<unsigned>& dimension) override;
        GLuint GetFrameBufferHandle() const { return m_frameBufferHandle; }

    protected:
        void CreateFrameBufferTexture(const MathLib::Dimension<unsigned>& dimension, const Graphics::GraphicFormat& fmt);

    protected:
        GLuint m_frameBufferHandle;
    };
}

#endif
#endif // BACK_SURFACE_EGL_H
