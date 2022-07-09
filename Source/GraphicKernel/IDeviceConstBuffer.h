/*********************************************************************
 * \file   IDeviceConstBuffer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_CONST_BUFFER_INTERFACE_H
#define DEVICE_CONST_BUFFER_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <memory>
#include <cassert>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IDeviceConstBuffer : public std::enable_shared_from_this<IDeviceConstBuffer>
    {
    public:
        IDeviceConstBuffer(const std::string& name);
        IDeviceConstBuffer(const IDeviceConstBuffer&) = delete;
        IDeviceConstBuffer(IDeviceConstBuffer&&) = delete;
        virtual ~IDeviceConstBuffer();
        IDeviceConstBuffer& operator=(const IDeviceConstBuffer&) = delete;
        IDeviceConstBuffer& operator=(IDeviceConstBuffer&&) = delete;

        virtual error CreateDataBuffer(unsigned int cb_size);
        virtual future_error AsyncCreateDataBuffer(unsigned int cb_size);

        virtual unsigned int GetSize() { return m_size; };
        virtual unsigned char* GetDataBuffer() { assert(!m_data.empty());  return &m_data[0]; };

    protected:
        std::string m_name;
        unsigned int m_size;
        byte_buffer m_data;
    };
    using IDeviceConstBufferPtr = std::shared_ptr<IDeviceConstBuffer>;
    using IDeviceConstBufferWeak = std::weak_ptr<IDeviceConstBuffer>;
}

#endif // DEVICE_CONST_BUFFER_INTERFACE_H
