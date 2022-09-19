#include "RenderBuffer.h"
#include "EngineErrors.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "GraphicKernel/IGraphicAPI.h"
#include <cassert>

using namespace Enigma::Engine;

RenderBuffer::RenderBuffer(const RenderBufferSignature& signature,
    const Graphics::IVertexBufferPtr& vertex_buffer, const Graphics::IIndexBufferPtr& index_buffer)
{
    m_signature = signature;
    m_vertexBuffer = vertex_buffer;
    m_indexBuffer = index_buffer;
    m_isDataEmpty = true;
}

RenderBuffer::~RenderBuffer()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
}

error RenderBuffer::UpdateVertex(const byte_buffer& dataBuffer, const uint_buffer& indexBuffer)
{
    assert(m_vertexBuffer);
    if (FATAL_LOG_EXPR(dataBuffer.size() > m_vertexBuffer->BufferSize())) return Graphics::ErrorCode::bufferSize;
    if (m_indexBuffer)
    {
        unsigned int dataSize = (unsigned int)indexBuffer.size() * sizeof(unsigned int);
        if (FATAL_LOG_EXPR(dataSize > m_indexBuffer->BufferSize())) return Graphics::ErrorCode::bufferSize;
    }
    if (Graphics::IGraphicAPI::Instance()->UseAsync())
    {
        
    }
    return ErrorCode::ok;
}
