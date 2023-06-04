#include "TextureEgl.h"
#include "BackSurfaceEgl.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/Rect.h"
#include <cassert>
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "png.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

extern GLint GraphicFormatToGLSizedFormat(const Enigma::Graphics::GraphicFormat& fmt);
extern GLenum GraphicFormatToGLFormat(const Enigma::Graphics::GraphicFormat& fmt);
extern GLenum GraphicFormatToGLPixelType(const Enigma::Graphics::GraphicFormat& fmt);

TextureEgl::TextureEgl(const std::string& name) : ITexture(name)
{
    m_texture = 0;
}

TextureEgl::~TextureEgl()
{
    if (m_texture != 0)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
}

error TextureEgl::CreateFromSystemMemory(const MathLib::Dimension& dimension, const byte_buffer& buff)
{
    if (m_texture != 0)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    glGenTextures(1, &m_texture);
    if (!buff.empty())
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(dimension.m_width), static_cast<GLsizei>(dimension.m_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, &buff[0]);
        // mip map 要記得產生, mip filter 要用
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_dimension = dimension;
    m_format = Graphics::GraphicFormat::FMT_A8R8G8B8;

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceFromMemoryCreated>(m_name));

    return ErrorCode::ok;
}

error TextureEgl::LoadTextureImage(const byte_buffer& img_buff)
{
    if (FATAL_LOG_EXPR(img_buff.empty()))
    {
        Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceLoadImageFailed>(
            m_name, ErrorCode::nullMemoryBuffer));
        return ErrorCode::nullMemoryBuffer;
    }
    if (png_check_sig((const unsigned char*)&img_buff[0], 8))
    {
        png_image image;
        memset(&image, 0, sizeof(image));
        image.version = PNG_IMAGE_VERSION;
        int res = png_image_begin_read_from_memory(&image, &img_buff[0], img_buff.size());
        if (res == 0) return ErrorCode::eglLoadTexture;
        byte_buffer raw_buffer;
        image.format = PNG_FORMAT_RGBA;
        unsigned int raw_buffer_size = PNG_IMAGE_SIZE(image);
        raw_buffer.resize(raw_buffer_size);
        res = png_image_finish_read(&image, NULL, &raw_buffer[0], 0, NULL);
        if (res == 0)
        {
            Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceLoadImageFailed>(
                m_name, ErrorCode::eglLoadTexture));
            return ErrorCode::eglLoadTexture;
        }

        CreateFromSystemMemory(MathLib::Dimension{ image.width, image.height }, raw_buffer);
        png_image_free(&image);

        Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceImageLoaded>(m_name));

        return ErrorCode::ok;
    }
    return ErrorCode::pngFileFormat;
}

error TextureEgl::RetrieveTextureImage(const MathLib::Rect& rcSrc)
{
    return ErrorCode::notImplement;
}

error TextureEgl::UpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff)
{
    if (FATAL_LOG_EXPR(img_buff.empty()))
    {
        Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceUpdateImageFailed>(
            m_name, ErrorCode::nullMemoryBuffer));
        return ErrorCode::nullMemoryBuffer;
    }
    if (FATAL_LOG_EXPR((rcDest.Width() <= 0) || (rcDest.Height() <= 0)))
    {
        Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceUpdateImageFailed>(
            m_name, ErrorCode::invalidParameter));
        return ErrorCode::invalidParameter;
    }
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        rcDest.Left(), static_cast<GLint>(m_dimension.m_height - rcDest.Bottom()),
        rcDest.Width(), rcDest.Height(),
        GL_RGBA, GL_UNSIGNED_BYTE, &img_buff[0]);
    glBindTexture(GL_TEXTURE_2D, 0);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceImageUpdated>(m_name, rcDest));

    return ErrorCode::ok;
}

error TextureEgl::SaveTextureImage(const FileSystem::IFilePtr& file)
{
    if (FATAL_LOG_EXPR(m_texture == 0)) return ErrorCode::nullEglTexture;
    if (FATAL_LOG_EXPR(m_isCubeTexture)) return ErrorCode::saveTextureFile;
    Platforms::Debug::Printf("GLES not support GetTexImage");
    /* glBindTexture(GL_TEXTURE_2D, m_texture);
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    image.height = m_height;
    image.width = m_width;
    unsigned char* raw_buffer;
    image.format = PNG_FORMAT_RGBA;
    unsigned int raw_buffer_size = PNG_IMAGE_SIZE(image);
    raw_buffer = EN_ALLOC(unsigned char, raw_buffer_size);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw_buffer);

    m_imageFileName = filename;

    png_image_write_to_file(&image, FileSystem::Instance()->GetStdioFullPath(filename, pathid).c_str(),
        0, raw_buffer, 0, 0); */

    return ErrorCode::ok;
}

error TextureEgl::UseAsBackSurface(const std::shared_ptr<Graphics::IBackSurface>& back_surf, const std::vector<Graphics::RenderTextureUsage>& usages)
{
    assert(m_texture != 0);
    BackSurfaceEgl* bb = dynamic_cast<BackSurfaceEgl*>(back_surf.get());
    assert(bb);
    MathLib::Dimension dimension = bb->GetDimension();
    glBindTexture(GL_TEXTURE_2D, m_texture);
    unsigned int data_size = dimension.m_width * dimension.m_height * 4;
    byte_buffer data;
    data.resize(data_size);
    glTexImage2D(GL_TEXTURE_2D, 0, GraphicFormatToGLSizedFormat(m_format),
        static_cast<GLsizei>(dimension.m_width), static_cast<GLsizei>(dimension.m_height),
        0, GraphicFormatToGLFormat(m_format), GraphicFormatToGLPixelType(m_format), &data[0]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
    // 這幾個Texture Parameter 是必要的。同時，因為這個 Texture 並沒有 MipMap, 所以在 Texture Parameter &
    // Sampler Parameter 中，Mip Filter 要為 None.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_dimension = dimension;

    glBindFramebuffer(GL_FRAMEBUFFER, bb->GetFrameBufferHandle());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::TextureResourceAsBackSurfaceUsed>(m_name, back_surf->GetName()));

    return ErrorCode::ok;
}

#endif