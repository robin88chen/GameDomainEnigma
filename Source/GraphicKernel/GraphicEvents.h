/*********************************************************************
 * \file   GraphicEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_EVENTS_H
#define GRAPHIC_EVENTS_H

#include "Frameworks/Event.h"
#include "MathLib/Rect.h"

namespace Enigma::Graphics
{
    //------------------------ Device --------------------//
    class DeviceCreated : public Frameworks::IEvent
    {
    };

    //----------------------- Surfaces -------------------//
    class PrimarySurfaceCreated : public Frameworks::IEvent
    {
    public:
        PrimarySurfaceCreated(const std::string& back_name, const std::string& depth_name) :
            m_backSurfaceName(back_name), m_depthSurfaceName(depth_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_backSurfaceName;
        std::string m_depthSurfaceName;
    };
    class BackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        BackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class MultiBackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        MultiBackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class DepthSurfaceCreated : public Frameworks::IEvent
    {
    public:
        DepthSurfaceCreated(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };
    class DepthSurfaceShared : public Frameworks::IEvent
    {
    public:
        DepthSurfaceShared(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };

    //---------------- Textures -----------------------//
    class DeviceTextureCreated : public Frameworks::IEvent
    {
    public:
        DeviceTextureCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class DeviceMultiTextureCreated : public Frameworks::IEvent
    {
    public:
        DeviceMultiTextureCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceFromMemoryCreated : public Frameworks::IEvent
    {
    public:
        TextureResourceFromMemoryCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceImageLoaded : public Frameworks::IEvent
    {
    public:
        TextureResourceImageLoaded(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class MultiTextureResourceImagesLoaded : public Frameworks::IEvent
    {
    public:
        MultiTextureResourceImagesLoaded(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceImageSaved : public Frameworks::IEvent
    {
    public:
        TextureResourceImageSaved(const std::string& texture_name, const std::string& full_filename) :
            m_textureName(texture_name), m_fullFilename(full_filename) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetFullFilename() { return m_fullFilename; }
    private:
        std::string m_textureName;
        std::string m_fullFilename;
    };
    class MultiTextureResourceImagesSaved : public Frameworks::IEvent
    {
    public:
        MultiTextureResourceImagesSaved(const std::string& texture_name, const std::vector<std::string>& full_filenames) :
            m_textureName(texture_name), m_fullFilenames(full_filenames) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::vector<std::string>& GetFullFilenames() { return m_fullFilenames; }
    private:
        std::string m_textureName;
        std::vector<std::string>m_fullFilenames;
    };
    class TextureResourceImageRetrieved : public Frameworks::IEvent
    {
    public:
        TextureResourceImageRetrieved(const std::string& texture_name, const MathLib::Rect& rc) :
            m_textureName(texture_name), m_imageRect(rc) {};
        const std::string& GetTextureName() { return m_textureName; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
    private:
        std::string m_textureName;
        MathLib::Rect m_imageRect;
    };
    class TextureResourceImageUpdated : public Frameworks::IEvent
    {
    public:
        TextureResourceImageUpdated(const std::string& texture_name, const MathLib::Rect& rc) :
            m_textureName(texture_name), m_imageRect(rc) {};
        const std::string& GetTextureName() { return m_textureName; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
    private:
        std::string m_textureName;
        MathLib::Rect m_imageRect;
    };
    class TextureResourceAsBackSurfaceUsed : public Frameworks::IEvent
    {
    public:
        TextureResourceAsBackSurfaceUsed(const std::string& texture_name, const std::string& backsurface_name) :
            m_textureName(texture_name), m_backSurfaceName(backsurface_name) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_textureName;
        std::string m_backSurfaceName;
    };
    class MultiTextureResourcesAsBackSurfaceUsed : public Frameworks::IEvent
    {
    public:
        MultiTextureResourcesAsBackSurfaceUsed(const std::string& texture_name, const std::string& backsurface_name) :
            m_textureName(texture_name), m_backSurfaceName(backsurface_name) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_textureName;
        std::string m_backSurfaceName;
    };

    //---------------- Vertex/Index Buffers -----------------------//
    class DeviceVertexBufferCreated : public Frameworks::IEvent
    {
    public:
        DeviceVertexBufferCreated(const std::string& buff_name) :
            m_bufferName(buff_name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class DeviceIndexBufferCreated : public Frameworks::IEvent
    {
    public:
        DeviceIndexBufferCreated(const std::string& buff_name) :
            m_bufferName(buff_name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class VertexBufferResourceCreated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceCreated(const std::string& vb_name) :
            m_vertexBufferName(vb_name) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
    private:
        std::string m_vertexBufferName;
    };
    class VertexBufferResourceUpdated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceUpdated(const std::string& vb_name) :
            m_vertexBufferName(vb_name) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
    private:
        std::string m_vertexBufferName;
    };
    class VertexBufferResourceRangedUpdated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceRangedUpdated(const std::string& vb_name, unsigned int offset, unsigned int count) :
            m_vertexBufferName(vb_name), m_offset(offset), m_count(count) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
        unsigned int GetVertexOffset() { return m_offset; }
        unsigned int GetVertexCount() { return m_count; }
    private:
        std::string m_vertexBufferName;
        unsigned int m_offset;
        unsigned int m_count;
    };
    class IndexBufferResourceCreated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceCreated(const std::string& ib_name) :
            m_indexBufferName(ib_name) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
    private:
        std::string m_indexBufferName;
    };
    class IndexBufferResourceUpdated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceUpdated(const std::string& ib_name) :
            m_indexBufferName(ib_name) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
    private:
        std::string m_indexBufferName;
    };
    class IndexBufferResourceRangedUpdated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceRangedUpdated(const std::string& ib_name, unsigned int offset, unsigned int count) :
            m_indexBufferName(ib_name), m_offset(offset), m_count(count) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
        unsigned int GetIndexOffset() { return m_offset; }
        unsigned int GetIndexCount() { return m_count; }
    private:
        std::string m_indexBufferName;
        unsigned int m_offset;
        unsigned int m_count;
    };

}

#endif // GRAPHIC_EVENTS_H
