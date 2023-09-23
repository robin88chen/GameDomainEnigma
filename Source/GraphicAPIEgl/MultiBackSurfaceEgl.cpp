#include "MultiBackSurfaceEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/MemoryAllocMacro.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

MultiBackSurfaceEgl::MultiBackSurfaceEgl(const std::string& name, const MathLib::Dimension<unsigned>& dimension,
    unsigned buffer_count, const std::vector<Graphics::GraphicFormat>& fmt) : IMultiBackSurface(name)
{
    m_dimension = dimension;
    m_formatArray = fmt;
    m_surfaceCount = buffer_count;
    m_format = fmt[0];
    m_frameBufferHandle = 0;
    CreateFrameBufferTexture(m_dimension, m_format);
}

MultiBackSurfaceEgl::~MultiBackSurfaceEgl()
{
    if (m_frameBufferHandle != 0)
    {
        glDeleteFramebuffers(1, &m_frameBufferHandle);
    }
}

error MultiBackSurfaceEgl::Resize(const MathLib::Dimension<unsigned>& dimension)
{
    m_dimension = dimension;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::BackSurfaceResized>(m_name, m_dimension));
    return ErrorCode::ok;
}

void MultiBackSurfaceEgl::CreateFrameBufferTexture(const MathLib::Dimension<unsigned>& dimension, const Graphics::GraphicFormat& fmt)
{
    glGenFramebuffers(1, &m_frameBufferHandle);
}

#endif
