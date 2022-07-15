/*********************************************************************
 * \file   TextureSamplerBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef TEXTURE_SAMPLER_BUILDER_H
#define TEXTURE_SAMPLER_BUILDER_H
#include <Frameworks/Event.h>
#include <Frameworks/EventSubscriber.h>
#include <GraphicKernel/IGraphicAPI.h>

class TextureSamplerBuilder
{
public:
    TextureSamplerBuilder(Enigma::Graphics::IGraphicAPI::AsyncType asyncType);
    TextureSamplerBuilder(const TextureSamplerBuilder&) = delete;
    TextureSamplerBuilder(TextureSamplerBuilder&&) = delete;
    ~TextureSamplerBuilder();
    const TextureSamplerBuilder& operator=(const TextureSamplerBuilder&) = delete;
    const TextureSamplerBuilder& operator=(TextureSamplerBuilder&&) = delete;

    void BuildTexture(const std::string& name, const std::string& filename, const std::string& path_id);

private:
    void OnTextureCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Graphics::IGraphicAPI::AsyncType m_async;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureImageLoaded;

    std::string m_textureName;
    std::string m_textureFilename;
    std::string m_texturePathId;
};
class TextureLoaded : public Enigma::Frameworks::IEvent
{
public:
    TextureLoaded(const std::string& name) :
        m_name(name) {};
    const std::string& GetTextureName() { return m_name; }
private:
    std::string m_name;
};

#endif // TEXTURE_SAMPLER_BUILDER_H
