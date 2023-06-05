/*********************************************************************
 * \file   MultiTextureEgl.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef MULTI_TEXTURE_EGL_H
#define MULTI_TEXTURE_EGL_H

#include "GraphicKernel/IMultiTexture.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"
#include <vector>

namespace Enigma::Devices
{
    using error = std::error_code;

    class MultiTextureEgl : public Graphics::IMultiTexture
    {
    public:
        MultiTextureEgl(const std::string& name);
        MultiTextureEgl(const MultiTextureEgl&) = delete;
        MultiTextureEgl(MultiTextureEgl&&) = delete;
        virtual ~MultiTextureEgl() override;
        MultiTextureEgl& operator=(const MultiTextureEgl&) = delete;
        MultiTextureEgl& operator=(MultiTextureEgl&&) = delete;

        virtual error UseAsBackSurface(const Graphics::IBackSurfacePtr& back_surf, const std::vector<Graphics::RenderTextureUsage>& usages) override;

        GLuint GetTextureHandle(unsigned int idx) const;
        const std::vector<GLuint>& GetTextureHandlesArray() const;
        unsigned int GetTextureHandleCount() const { return (unsigned int)m_textures.size(); }

    protected:
        virtual error LoadTextureImages(const std::vector<byte_buffer>& img_buffs) override;
        virtual error SaveTextureImages(const std::vector<FileSystem::IFilePtr>& files) override;

        virtual error CreateFromSystemMemories(const MathLib::Dimension& dimension, unsigned count, const std::vector<byte_buffer>& buffs) override;
        error CreateOneFromSystemMemory(unsigned int index, const MathLib::Dimension& dimension,
            const byte_buffer& buff);
    protected:
        std::vector<GLuint> m_textures;
    };
}

#endif

#endif // MULTI_TEXTURE_EGL_H
