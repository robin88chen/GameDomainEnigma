#include "DepthStencilSurfaceEgl.h"
#include "BackSurfaceEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include <cassert>

#if TARGET_PLATFORM == PLATFORM_ANDROID

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DepthStencilSurfaceEgl::DepthStencilSurfaceEgl(const std::string& name, const MathLib::Dimension<unsigned>& dimension, 
    const Graphics::GraphicFormat& fmt) : IDepthStencilSurface(name)
{
    m_dimension = dimension;
    m_format = fmt;
    m_frameBufferHandle = 0;
}

DepthStencilSurfaceEgl::DepthStencilSurfaceEgl(const std::string& name, const std::shared_ptr<DepthStencilSurfaceEgl>& shared_depth)
    : IDepthStencilSurface(name)
{
    assert(shared_depth);
    m_dimension = shared_depth->m_dimension;
    m_format = shared_depth->m_format;
    m_frameBufferHandle = shared_depth->m_frameBufferHandle;
}

DepthStencilSurfaceEgl::~DepthStencilSurfaceEgl()
{
    if (m_frameBufferHandle)
    {
        glDeleteRenderbuffers(1, &m_frameBufferHandle);
    }
}

void DepthStencilSurfaceEgl::MakeBackSurfaceRelated(const Graphics::IBackSurfacePtr& back_surf)
{
    if (!back_surf) return;
    BackSurfaceEgl* bb = dynamic_cast<BackSurfaceEgl*>(back_surf.get());
    if (FATAL_LOG_EXPR(!bb)) return;
    if (m_frameBufferHandle == 0)
    {
        glGenRenderbuffers(1, &m_frameBufferHandle);
        glBindRenderbuffer(GL_RENDERBUFFER, m_frameBufferHandle);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(m_dimension.m_width), static_cast<GLsizei>(m_dimension.m_height));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, bb->GetFrameBufferHandle());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_frameBufferHandle);
}

error DepthStencilSurfaceEgl::Resize(const MathLib::Dimension<unsigned>& dimension)
{
    m_dimension = dimension;
    Frameworks::EventPublisher::post(std::make_shared<Graphics::DepthSurfaceResized>(m_name, m_dimension));
    return ErrorCode::ok;
}

#endif
