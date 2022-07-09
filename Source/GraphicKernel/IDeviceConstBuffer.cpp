#include "IDeviceConstBuffer.h"
#include "GraphicErrors.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include <cassert>

using namespace Enigma::Graphics;

IDeviceConstBuffer::IDeviceConstBuffer(const std::string& name) : m_name(name)
{
    m_size = 0;
}

IDeviceConstBuffer::~IDeviceConstBuffer()
{
}

error IDeviceConstBuffer::CreateDataBuffer(unsigned int cb_size)
{
    assert(cb_size > 0);
    m_size = cb_size;
    m_data.resize(cb_size);
    memset(&m_data[0], 0, sizeof(unsigned char) * cb_size);
    return ErrorCode::ok;
}

future_error IDeviceConstBuffer::AsyncCreateDataBuffer(unsigned cb_size)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask(std::bind(&IDeviceConstBuffer::CreateDataBuffer, this, cb_size));
}
