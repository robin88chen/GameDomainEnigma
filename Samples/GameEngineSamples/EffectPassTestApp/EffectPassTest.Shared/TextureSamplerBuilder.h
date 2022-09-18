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
#include <GraphicKernel/IDeviceSamplerState.h>
#include <GraphicKernel/IGraphicAPI.h>

class TextureSamplerBuilder
{
public:
    TextureSamplerBuilder();
    TextureSamplerBuilder(const TextureSamplerBuilder&) = delete;
    TextureSamplerBuilder(TextureSamplerBuilder&&) = delete;
    ~TextureSamplerBuilder();
    const TextureSamplerBuilder& operator=(const TextureSamplerBuilder&) = delete;
    const TextureSamplerBuilder& operator=(TextureSamplerBuilder&&) = delete;

    void BuildTexture(const std::string& name, const std::string& filename, const std::string& path_id);
    void BuildSampler(const std::string& name, const Enigma::Graphics::IDeviceSamplerState::SamplerStateData data);

private:
    void OnTextureCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);

    void OnSamplerCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnSamplerResourceCreated(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Frameworks::EventSubscriberPtr m_onTextureCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureImageLoaded;

    Enigma::Frameworks::EventSubscriberPtr m_onSamplerCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSamplerResourceCreated;

    std::string m_textureName;
    std::string m_textureFilename;
    std::string m_texturePathId;
    std::string m_samplerName;
    Enigma::Graphics::IDeviceSamplerState::SamplerStateData m_samplerData;
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
class SamplerStateBuilt : public Enigma::Frameworks::IEvent
{
public:
    SamplerStateBuilt(const std::string& name) :
        m_name(name) {};
    const std::string& GetSamplerName() { return m_name; }
private:
    std::string m_name;
};

#endif // TEXTURE_SAMPLER_BUILDER_H
