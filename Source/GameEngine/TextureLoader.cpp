#include "EngineErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IMultiTexture.h"
#include "GraphicKernel/ITexture.h"
#include "Platforms/PlatformLayer.h"
#include "TextureLoader.h"
#include "TextureRepository.h"
#include <cassert>
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::Frameworks;

TextureLoader::TextureLoader()
{
    m_onTextureCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onDeviceTextureCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onMultiTextureCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onDeviceTextureCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceMultiTextureCreated), m_onMultiTextureCreated);
    m_onTextureImageLoaded = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureImageLoaded(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureLoadImageFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureLoadImageFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
    m_onTextureResourceCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureResourceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceFromMemoryCreated), m_onTextureResourceCreated);
    m_onMultiTextureResourceCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureResourceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::MultiTextureResourceFromMemoryCreated), m_onMultiTextureResourceCreated);
    m_onTextureCreateResourceFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureCreateResourceFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceCreateFromMemoryFailed), m_onTextureCreateResourceFailed);
}

TextureLoader::~TextureLoader()
{
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceMultiTextureCreated), m_onMultiTextureCreated);
    m_onMultiTextureCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureImageLoaded = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
    m_onTextureLoadImageFailed = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceFromMemoryCreated), m_onTextureResourceCreated);
    m_onTextureResourceCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::MultiTextureResourceFromMemoryCreated), m_onMultiTextureResourceCreated);
    m_onMultiTextureResourceCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceCreateFromMemoryFailed), m_onTextureCreateResourceFailed);
    m_onTextureCreateResourceFailed = nullptr;
}

void TextureLoader::loadImage(const std::shared_ptr<Texture>& texture, const TextureDto& dto)
{
    assert(texture);
    m_contentingTexture = texture;
    m_textureDto = dto;
    if (m_contentingTexture->isMultiTexture())
    {
        CommandBus::post(std::make_shared<CreateDeviceMultiTexture>(m_contentingTexture->id().name()));
    }
    else
    {
        CommandBus::post(std::make_shared<CreateDeviceTexture>(m_contentingTexture->id().name()));
    }
}

void TextureLoader::loadResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex)
{
    assert(m_contentingTexture);
    assert(m_contentingTexture->lazyStatus().isLoading());
    assert(dev_tex);
    if (m_contentingTexture->isMultiTexture())
    {
        std::dynamic_pointer_cast<IMultiTexture>(dev_tex)->multiLoad(m_textureDto.filePaths(), {});
    }
    else
    {
        dev_tex->load(m_textureDto.filePaths()[0], "");
    }
}

void TextureLoader::createEmptyResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex)
{
    assert(m_contentingTexture);
    assert(m_contentingTexture->lazyStatus().isLoading());
    assert(dev_tex);
    if (m_contentingTexture->isMultiTexture())
    {
        std::vector<byte_buffer> buffers;
        buffers.resize(m_textureDto.surfaceCount());
        std::dynamic_pointer_cast<IMultiTexture>(dev_tex)->multiCreate(m_textureDto.dimension(), m_textureDto.surfaceCount(), buffers);
    }
    else
    {
        dev_tex->create(m_textureDto.dimension(), byte_buffer{});
    }
}

void TextureLoader::onDeviceTextureCreated(const IEventPtr& e)
{
    if (!m_contentingTexture) return;
    if (!e) return;
    std::string tex_name;
    if (auto ev = std::dynamic_pointer_cast<DeviceTextureCreated, IEvent>(e))
    {
        tex_name = ev->textureName();
    }
    else if (auto ev = std::dynamic_pointer_cast<DeviceMultiTextureCreated, IEvent>(e))
    {
        tex_name = ev->textureName();
    }
    else return;
    if (tex_name != m_contentingTexture->id().name()) return;

    auto texture = IGraphicAPI::instance()->TryFindGraphicAsset<ITexturePtr>(tex_name);
    if (!texture)
    {
        Platforms::Debug::Printf("can't get texture asset %s", tex_name.c_str());
        failLoadingImage(ErrorCode::findStashedAssetFail);
        return;
    }
    if (!m_textureDto.filePaths().empty())
    {
        loadResourceTextures(texture.value());
    }
    else
    {
        createEmptyResourceTextures(texture.value());
    }
}

void TextureLoader::onTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!m_contentingTexture) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageLoaded, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->textureName() != m_contentingTexture->id().name()) return;
    Graphics::ITexturePtr dev_tex = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::ITexturePtr>(m_contentingTexture->id().name());
    if (!dev_tex)
    {
        Platforms::Debug::Printf("can't get texture asset %s", ev->textureName().c_str());
        failLoadingImage(ErrorCode::findStashedAssetFail);
        return;
    }
    m_contentingTexture->instanceDeviceTexture(dev_tex);
    Frameworks::EventPublisher::post(std::make_shared<TextureLoaded>(m_contentingTexture->id(), m_contentingTexture));
    m_contentingTexture = nullptr;
}

void TextureLoader::onTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!m_contentingTexture) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceLoadImageFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->textureName() != m_contentingTexture->id().name()) return;
    Platforms::Debug::Printf("texture %s load image %s failed", m_contentingTexture->id().name().c_str(), m_textureDto.filePaths()[0].c_str());
    failLoadingImage(ev->error());
}

void TextureLoader::onTextureResourceCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!m_contentingTexture) return;
    if (!e) return;
    std::string tex_name;
    if (auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceFromMemoryCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->textureName();
    }
    else if (auto ev = std::dynamic_pointer_cast<Graphics::MultiTextureResourceFromMemoryCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->textureName();
    }
    else return;
    if (tex_name != m_contentingTexture->id().name()) return;
    Graphics::ITexturePtr dev_tex = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::ITexturePtr>(m_contentingTexture->id().name());
    if (!dev_tex)
    {
        Platforms::Debug::Printf("can't get texture asset %s", tex_name.c_str());
        failLoadingImage(ErrorCode::findStashedAssetFail);
        return;
    }
    m_contentingTexture->instanceDeviceTexture(dev_tex);
    Frameworks::EventPublisher::post(std::make_shared<TextureLoaded>(m_contentingTexture->id(), m_contentingTexture));
    m_contentingTexture = nullptr;
}

void TextureLoader::onTextureCreateResourceFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!m_contentingTexture) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceCreateFromMemoryFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->textureName() != m_contentingTexture->id().name()) return;
    Platforms::Debug::Printf("texture %s create from memory failed", m_contentingTexture->id().name().c_str());
    failLoadingImage(ev->error());
}

void TextureLoader::failLoadingImage(std::error_code er)
{
    assert(m_contentingTexture);
    EventPublisher::post(std::make_shared<LoadTextureFailed>(m_contentingTexture->id(), er));
    m_contentingTexture = nullptr;
}
