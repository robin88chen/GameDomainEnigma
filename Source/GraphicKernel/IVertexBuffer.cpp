#include "IVertexBuffer.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IVertexBuffer::IVertexBuffer(const std::string& name)
{
    m_name = name;
    m_bufferSize = 0;
    m_sizeofVertex = 0;
}

IVertexBuffer::~IVertexBuffer()
{
}

future_error IVertexBuffer::AsyncCreate(unsigned sizeofVertex, unsigned sizeBuffer)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->Create(sizeofVertex, sizeBuffer); });
}

future_error IVertexBuffer::AsyncUpdate(const byte_buffer& dataVertex)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->Update(dataVertex); });
}

future_error IVertexBuffer::AsyncRangedUpdate(const ranged_buffer& buffer)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->RangedUpdate(buffer); });
}
