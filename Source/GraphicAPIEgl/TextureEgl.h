/*********************************************************************
 * \file   TextureEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef TEXTURE_EGL_H
#define TEXTURE_EGL_H

#include "GraphicKernel/ITexture.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class TextureEgl : public Graphics::ITexture
    {
    public:
        TextureEgl(const std::string& name);
        TextureEgl(const TextureEgl&) = delete;
        TextureEgl(TextureEgl&&) = delete;
        virtual ~TextureEgl();
        TextureEgl& operator=(const TextureEgl&) = delete;
        TextureEgl& operator=(TextureEgl&&) = delete;

        virtual error CreateFromSystemMemory(const MathLib::Dimension& dimension, const byte_buffer& buff) override;
        virtual error LoadTextureImage(const byte_buffer& img_buff) override;
        virtual error RetrieveTextureImage(const MathLib::Rect& rcSrc) override;
        virtual error UpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff) override;
        virtual error SaveTextureImage(const FileSystem::IFilePtr& file) override;
        virtual error UseAsBackSurface(const std::shared_ptr<Graphics::IBackSurface>& back_surf) override;

        GLuint GetTextureHandle() const { return m_texture; }
    protected:
        GLuint m_texture;
    };
}

#endif
#endif // TEXTURE_EGL_H
