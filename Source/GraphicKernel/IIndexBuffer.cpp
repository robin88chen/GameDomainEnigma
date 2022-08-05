#include "IIndexBuffer.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IIndexBuffer::IIndexBuffer(const std::string& name)
{
    m_name = name;
    m_bufferSize = 0;
}

IIndexBuffer::~IIndexBuffer()
{
}

future_error IIndexBuffer::AsyncCreate(unsigned sizeBuffer)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), sizeBuffer, this]() -> error { return Create(sizeBuffer); });
}

future_error IIndexBuffer::AsyncUpdate(const uint_buffer& dataIndex)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dataIndex, this]() -> error { return Update(dataIndex); });
}

future_error IIndexBuffer::AsyncRangedUpdate(const ranged_buffer& buffer)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), buffer, this]() -> error { return RangedUpdate(buffer); });
}
