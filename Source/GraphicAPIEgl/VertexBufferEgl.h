/*********************************************************************
 * \file   VertexBufferEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_BUFFER_EGL_H
#define VERTEX_BUFFER_EGL_H

#include "Platforms/PlatformConfig.h"
#include "GraphicKernel/IVertexBuffer.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class VertexBufferEgl : public Graphics::IVertexBuffer
    {
    public:
        VertexBufferEgl(const std::string& name);
        VertexBufferEgl(const VertexBufferEgl&) = delete;
        VertexBufferEgl(VertexBufferEgl&&) = delete;
        virtual ~VertexBufferEgl();
        VertexBufferEgl& operator=(const VertexBufferEgl&) = delete;
        VertexBufferEgl& operator=(VertexBufferEgl&&) = delete;

        virtual error Create(unsigned int sizeofVertex, unsigned int sizeBuffer) override;
        virtual error Update(const byte_buffer& dataVertex) override;
        virtual error RangedUpdate(const ranged_buffer& buffer) override;

        GLuint GetBufferHandle() const { return m_bufferHandle; }
    protected:
        GLuint m_bufferHandle;
    };
};

#endif
#endif // VERTEX_BUFFER_EGL_H
