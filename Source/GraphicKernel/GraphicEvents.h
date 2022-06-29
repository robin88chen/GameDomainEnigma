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

}

#endif // GRAPHIC_EVENTS_H
