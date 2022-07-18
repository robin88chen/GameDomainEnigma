#include "BackSurfaceEgl.h"
#include "GraphicKernel/GraphicErrors.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

BackSurfaceEgl::BackSurfaceEgl(const std::string& name, const MathLib::Dimension& dimension, 
    const Graphics::GraphicFormat& fmt, bool primary) : IBackSurface(name, primary)
{
    m_dimension = dimension;
    m_format = fmt;

    m_frameBufferHandle = 0;
    if (!m_isPrimary)
    {
        CreateFrameBufferTexture(dimension, fmt);
    }
}

BackSurfaceEgl::~BackSurfaceEgl()
{
    if (m_frameBufferHandle != 0)
    {
        glDeleteFramebuffers(1, &m_frameBufferHandle);
    }
}

error BackSurfaceEgl::Resize(const MathLib::Dimension& dimension)
{
    m_dimension = dimension;
    return ErrorCode::ok;
}

void BackSurfaceEgl::CreateFrameBufferTexture(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    glGenFramebuffers(1, &m_frameBufferHandle);
}

#endif