#include "IVertexBuffer.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IVertexBuffer::IVertexBuffer(const std::string& name, unsigned int sizeofVertex, unsigned int sizeBuffer)
{
    m_name = name;
    m_bufferSize = sizeBuffer;
    m_sizeofVertex = sizeofVertex;
}

IVertexBuffer::~IVertexBuffer()
{
}

future_error IVertexBuffer::AsyncUpdate(const byte_buffer& dataVertex)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dataVertex, this]() -> error { return Update(dataVertex); });
}

future_error IVertexBuffer::AsyncRangedUpdate(const ranged_buffer& buffer)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), buffer, this]() -> error { return RangedUpdate(buffer); });
}
