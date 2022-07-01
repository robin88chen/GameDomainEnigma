/*********************************************************************
 * \file   IIndexBuffer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef INDEX_BUFFER_INTERFACE_H
#define INDEX_BUFFER_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <memory>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IIndexBuffer : public std::enable_shared_from_this<IIndexBuffer>
    {
    public:
        struct ranged_buffer
        {
            unsigned int idx_offset;
            unsigned int idx_count;
            uint_buffer data;
        };
    public:
        IIndexBuffer(const std::string& name);
        IIndexBuffer(const IIndexBuffer&) = delete;
        IIndexBuffer(IIndexBuffer&&) = delete;
        virtual ~IIndexBuffer();
        IIndexBuffer& operator=(const IIndexBuffer&) = delete;
        IIndexBuffer& operator=(IIndexBuffer&&) = delete;

        const std::string& GetName() { return m_name; }

        virtual error Create(unsigned int sizeBuffer) = 0;
        virtual future_error AsyncCreate(unsigned int sizeBuffer);
        virtual error Update(const uint_buffer& dataIndex) = 0;
        virtual future_error AsyncUpdate(const uint_buffer& dataIndex);
        virtual error RangedUpdate(const ranged_buffer& buffer) = 0;
        virtual future_error AsyncRangedUpdate(const ranged_buffer& buffer);

        // Buffer size
        virtual unsigned int BufferSize() { return m_bufferSize; };
        virtual int IndexSize() { return sizeof(unsigned int); };
        virtual unsigned int IndexCount() { return m_bufferSize / sizeof(unsigned int); };

    protected:
        std::string m_name;
        unsigned int m_bufferSize;
    };
    using IIndexBufferPtr = std::shared_ptr<IIndexBuffer>;
    using IIndexBufferWeak = std::weak_ptr<IIndexBuffer>;
}


#endif // INDEX_BUFFER_INTERFACE_H
