/*********************************************************************
 * \file   IVertexBuffer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef VERTEX_BUFFER_INTERFACE_H
#define VERTEX_BUFFER_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <memory>
#include <system_error>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IVertexBuffer : public std::enable_shared_from_this<IVertexBuffer>
    {
    public:
        struct ranged_buffer
        {
            unsigned int vtx_offset;
            unsigned int vtx_count;
            byte_buffer data;
        };
    public:
        IVertexBuffer(const std::string& name);
        IVertexBuffer(const IVertexBuffer&) = delete;
        IVertexBuffer(IVertexBuffer&&) = delete;
        virtual ~IVertexBuffer();
        IVertexBuffer& operator=(const IVertexBuffer&) = delete;
        IVertexBuffer& operator=(IVertexBuffer&&) = delete;

        const std::string& getName() { return m_name; }

        virtual error create(unsigned int sizeofVertex, unsigned int sizeBuffer) = 0;
        void update(const byte_buffer& dataVertex);
        void RangedUpdate(const ranged_buffer& buffer);

        // Buffer size
        virtual unsigned int BufferSize() { return m_bufferSize; };
        virtual unsigned int sizeofVertex() { return m_sizeofVertex; };

    protected:
        virtual error UpdateBuffer(const byte_buffer& dataVertex) = 0;
        virtual error RangedUpdateBuffer(const ranged_buffer& buffer) = 0;

    protected:
        std::string m_name;
        unsigned int m_bufferSize;
        unsigned int m_sizeofVertex;
    };
    using IVertexBufferPtr = std::shared_ptr<IVertexBuffer>;
    using IVertexBufferWeak = std::weak_ptr<IVertexBuffer>;
}


#endif // VERTEX_BUFFER_INTERFACE_H
