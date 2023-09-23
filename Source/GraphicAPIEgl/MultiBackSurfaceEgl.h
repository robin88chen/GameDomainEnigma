/*********************************************************************
 * \file   MultiBackSurfaceEgl.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef MULTI_BACK_SURFACE_EGL_H
#define MULTI_BACK_SURFACE_EGL_H

#include "Platforms/PlatformConfig.h"
#include "GraphicKernel/IMultiBackSurface.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>

namespace Enigma::Devices
{
    using error = std::error_code;
    class MultiBackSurfaceEgl : public Graphics::IMultiBackSurface
    {
    public:
        MultiBackSurfaceEgl(const std::string& name, const MathLib::Dimension<unsigned>& dimension,
            unsigned int buffer_count, const std::vector<Graphics::GraphicFormat>& fmt);
        MultiBackSurfaceEgl(const MultiBackSurfaceEgl&) = delete;
        MultiBackSurfaceEgl(MultiBackSurfaceEgl&&) = delete;
        virtual ~MultiBackSurfaceEgl() override;

        MultiBackSurfaceEgl& operator=(const MultiBackSurfaceEgl&) = delete;
        MultiBackSurfaceEgl& operator=(MultiBackSurfaceEgl&&) = delete;

        virtual error Resize(const MathLib::Dimension<unsigned>& dimension) override;
        GLuint GetFrameBufferHandle() const { return m_frameBufferHandle; }

    protected:
        void CreateFrameBufferTexture(const MathLib::Dimension<unsigned>& dimension, const Graphics::GraphicFormat& fmt);

    protected:
        GLuint m_frameBufferHandle;
    };
}

#endif


#endif // MULTI_BACK_SURFACE_EGL_H
