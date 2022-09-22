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

void IVertexBuffer::Update(const byte_buffer& dataVertex)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), dataVertex, this]() -> error { return UpdateBuffer(dataVertex); });
    }
    else
    {
        UpdateBuffer(dataVertex);
    }
}

void IVertexBuffer::RangedUpdate(const ranged_buffer& buffer)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), buffer, this]() -> error { return RangedUpdateBuffer(buffer); });
    }
    else
    {
        RangedUpdateBuffer(buffer);
    }
}
