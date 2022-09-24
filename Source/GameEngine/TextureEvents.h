/*********************************************************************
 * \file   TextureEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_EVENTS_H
#define TEXTURE_EVENTS_H

#include "Frameworks/Event.h"
#include "GraphicKernel/ITexture.h"
#include <string>
#include <system_error>

namespace Enigma::Engine
{
    class TextureLoaded : public Frameworks::IEvent
    {
    public:
        TextureLoaded(const std::string& name, const Graphics::ITexturePtr& tex) :
            m_name(name), m_texture(tex) {};
        const std::string& GetTextureName() const { return m_name; }
        const Graphics::ITexturePtr& GetTexture() const { return m_texture; }
    private:
        std::string m_name;
        Graphics::ITexturePtr m_texture;
    };
    class TextureLoadFailed : public Frameworks::IEvent
    {
    public:
        TextureLoadFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // TEXTURE_EVENTS_H
