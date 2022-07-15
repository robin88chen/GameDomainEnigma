#include "TextureSamplerBuilder.h"

#include <Frameworks/EventPublisher.h>
#include <GraphicKernel/GraphicEvents.h>
#include <Platforms/MemoryAllocMacro.h>

#include "GraphicKernel/ITexture.h"
#include <Platforms/PlatformLayer.h>

using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;

TextureSamplerBuilder::TextureSamplerBuilder(IGraphicAPI::AsyncType asyncType)
{
    m_async = asyncType;

    m_onTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceTextureCreated), m_onTextureCreated);
    m_onTextureImageLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureImageLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureResourceImageLoaded), m_onTextureImageLoaded);
}

TextureSamplerBuilder::~TextureSamplerBuilder()
{
    EventPublisher::Unsubscribe(typeid(DeviceTextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureImageLoaded = nullptr;
}

void TextureSamplerBuilder::BuildTexture(const std::string& name, const std::string& filename,
    const std::string& path_id)
{
    m_textureName = name;
    m_textureFilename = filename;
    m_texturePathId = path_id;
    if (m_async == IGraphicAPI::AsyncType::UseAsyncDevice)
    {
        IGraphicAPI::Instance()->AsyncCreateTexture(m_textureName);
    }
    else
    {
        IGraphicAPI::Instance()->CreateTexture(m_textureName);
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
    if (m_async == IGraphicAPI::AsyncType::UseAsyncDevice)
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
