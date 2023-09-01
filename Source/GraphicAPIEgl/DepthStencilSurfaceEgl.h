/*********************************************************************
 * \file   DepthStencilSurfaceEgl.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEPTH_STENCIL_SURFACE_EGL_H
#define DEPTH_STENCIL_SURFACE_EGL_H

#include "Platforms/PlatformConfig.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/IBackSurface.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>

namespace Enigma::Devices
{
    using error = std::error_code;
    class DepthStencilSurfaceEgl : public Graphics::IDepthStencilSurface
    {
    public:
        DepthStencilSurfaceEgl(const std::string& name, const MathLib::Dimension<unsigned>& dimension,
            const Graphics::GraphicFormat& fmt);
        DepthStencilSurfaceEgl(const std::string& name, const std::shared_ptr<DepthStencilSurfaceEgl>& shared_depth);
        DepthStencilSurfaceEgl(const DepthStencilSurfaceEgl&) = delete;
        DepthStencilSurfaceEgl(DepthStencilSurfaceEgl&&) = delete;
        virtual ~DepthStencilSurfaceEgl() override;

        DepthStencilSurfaceEgl& operator=(const DepthStencilSurfaceEgl&) = delete;
        DepthStencilSurfaceEgl& operator=(DepthStencilSurfaceEgl&&) = delete;

        virtual void MakeBackSurfaceRelated(const Graphics::IBackSurfacePtr& back_surf) override;
        virtual error Resize(const MathLib::Dimension<unsigned>& dimension) override;
        GLuint GetFrameBufferHandle() const { return m_frameBufferHandle; }

    protected:
        GLuint m_frameBufferHandle;
    };
}

#endif


#endif // DEPTH_STENCIL_SURFACE_EGL_H
