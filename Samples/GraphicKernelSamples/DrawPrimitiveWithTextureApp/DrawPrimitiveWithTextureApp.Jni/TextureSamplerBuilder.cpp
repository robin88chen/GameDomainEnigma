#include "TextureSamplerBuilder.h"

#include <Frameworks/EventPublisher.h>
#include <GraphicKernel/GraphicEvents.h>
#include <GraphicKernel/IDeviceSamplerState.h>
#include <Platforms/MemoryAllocMacro.h>

#include "GraphicKernel/ITexture.h"
#include <Platforms/PlatformLayer.h>

using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;

TextureSamplerBuilder::TextureSamplerBuilder()
{
    m_onTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceTextureCreated), m_onTextureCreated);
    m_onTextureImageLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureImageLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureResourceImageLoaded), m_onTextureImageLoaded);

    m_onSamplerCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnSamplerCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceSamplerStateCreated), m_onSamplerCreated);
    m_onSamplerResourceCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnSamplerResourceCreated(e); });
    EventPublisher::Subscribe(typeid(SamplerStateResourceCreated), m_onSamplerResourceCreated);
}

TextureSamplerBuilder::~TextureSamplerBuilder()
{
    EventPublisher::Unsubscribe(typeid(DeviceTextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureImageLoaded = nullptr;

    EventPublisher::Unsubscribe(typeid(DeviceSamplerStateCreated), m_onSamplerCreated);
    m_onSamplerCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SamplerStateResourceCreated), m_onSamplerResourceCreated);
    m_onSamplerResourceCreated = nullptr;
}

void TextureSamplerBuilder::BuildTexture(const std::string& name, const std::string& filename,
    const std::string& path_id)
{
    m_textureName = name;
    m_textureFilename = filename;
    m_texturePathId = path_id;
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->AsyncCreateTexture(m_textureName);
    }
    else
    {
        IGraphicAPI::Instance()->CreateTexture(m_textureName);
    }
}

void TextureSamplerBuilder::BuildSampler(const std::string& name, const IDeviceSamplerState::SamplerStateData data)
{
    m_samplerName = name;
    m_samplerData = data;
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->AsyncCreateSamplerState(m_samplerName);
    }
    else
    {
        IGraphicAPI::Instance()->CreateSamplerState(m_samplerName);
    }
}

void TextureSamplerBuilder::OnTextureCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceTextureCreated, IEvent>(e);
    if (!ev) return;
    ITexturePtr texture = IGraphicAPI::Instance()->GetGraphicAsset<ITexturePtr>(ev->GetTextureName());
    if (!texture)
    {
        Debug::Printf("can't get texture asset %s", ev->GetTextureName().c_str());
        return;
    }
    if (IGraphicAPI::Instance()->UseAsync())
    {
        texture->AsyncLoadTextureImage(m_textureFilename, m_texturePathId);
    }
    else
    {
        texture->LoadTextureImage(m_textureFilename, m_texturePathId);
    }
}

void TextureSamplerBuilder::OnTextureImageLoaded(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureResourceImageLoaded, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_textureName)
    {
        Debug::Printf("loaded texture name not match %s", ev->GetTextureName().c_str());
        return;
    }
    EventPublisher::Post(IEventPtr{ menew TextureLoaded{ m_textureName } });
}

void TextureSamplerBuilder::OnSamplerCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceSamplerStateCreated, IEvent>(e);
    if (!ev) return;
    IDeviceSamplerStatePtr sampler = IGraphicAPI::Instance()->GetGraphicAsset<IDeviceSamplerStatePtr>(ev->GetStateName());
    if (!sampler)
    {
        Debug::Printf("can't get sampler asset %s", ev->GetStateName().c_str());
        return;
    }
    if (IGraphicAPI::Instance()->UseAsync())
    {
        sampler->AsyncCreateFromData(m_samplerData);
    }
    else
    {
        sampler->CreateFromData(m_samplerData);
    }
}

void TextureSamplerBuilder::OnSamplerResourceCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<SamplerStateResourceCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetStateName() != m_samplerName)
    {
        Debug::Printf("created sampler name not match %s", ev->GetStateName().c_str());
        return;
    }
    EventPublisher::Post(IEventPtr{ menew SamplerStateBuilt{ m_samplerName } });
}