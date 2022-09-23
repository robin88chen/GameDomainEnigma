/*********************************************************************
 * \file   IndexBufferEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef INDEX_BUFFER_EGL_H
#define INDEX_BUFFER_EGL_H

#include "Platforms/PlatformConfig.h"
#include "GraphicKernel/IIndexBuffer.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class IndexBufferEgl : public Graphics::IIndexBuffer
    {
    public:
        IndexBufferEgl(const std::string& name);
        IndexBufferEgl(const IndexBufferEgl&) = delete;
        IndexBufferEgl(IndexBufferEgl&&) = delete;
        virtual ~IndexBufferEgl();
        IndexBufferEgl& operator=(const IndexBufferEgl&) = delete;
        IndexBufferEgl& operator=(IndexBufferEgl&&) = delete;

        virtual error Create(unsigned int sizeBuffer) override;

        GLuint GetBufferHandle() const { return m_bufferHandle; }

    protected:
        virtual error UpdateBuffer(const uint_buffer& dataIndex) override;
        virtual error RangedUpdateBuffer(const ranged_buffer& buffer) override;

    protected:
        GLuint m_bufferHandle;
    };
};

#endif
#endif // INDEX_BUFFER_EGL_H
