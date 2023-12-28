#include "TextureResourceProcessor.h"
#include "TextureLoader.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/EventPublisher.h"
#include "TextureDto.h"
#include "EngineErrors.h"
#include "TextureEvents.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Engine;

TextureResourceProcessor::TextureResourceProcessor()
{
    m_loader = menew TextureLoader();
    registerHandlers();
}

TextureResourceProcessor::~TextureResourceProcessor()
{
    SAFE_DELETE(m_loader);
    unregisterHandlers();
}

void TextureResourceProcessor::registerHandlers()
{
    m_onLoaderTextureLoaded = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onLoaderTextureLoaded(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureLoader::TextureLoaded), m_onLoaderTextureLoaded);
    m_onLoaderLoadTextureFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onLoaderLoadTextureFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoaderLoadTextureFailed);
}

void TextureResourceProcessor::unregisterHandlers()
{
    Frameworks::EventPublisher::unsubscribe(typeid(TextureLoader::TextureLoaded), m_onLoaderTextureLoaded);
    m_onLoaderTextureLoaded = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoaderLoadTextureFailed);
    m_onLoaderLoadTextureFailed = nullptr;
}

std::error_code TextureResourceProcessor::enqueueContentingDto(const std::shared_ptr<Texture>& texture, const GenericDto& dto)
{
    assert(texture);
    if (!texture->lazyStatus().isGhost()) return ErrorCode::textureAlreadyLoaded;
    std::lock_guard locker{ m_contentingQueueLock };
    m_contentingQueue.push({ texture, TextureDto::fromGenericDto(dto) });
    texture->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::InQueue);
    return ErrorCode::ok;
}

std::error_code TextureResourceProcessor::contentNextTextureResource()
{
    if (m_currentContentingTexture) return ErrorCode::ok;
    assert(m_loader);
    std::lock_guard locker{ m_contentingQueueLock };
    if (m_contentingQueue.empty())
    {
        m_currentContentingTexture = nullptr;
        return ErrorCode::ok;
    }
    auto& [texture, dto] = m_contentingQueue.front();
    m_currentContentingTexture = texture;
    m_loader->contentImage(texture, dto);
    texture->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::Loading);
    m_contentingQueue.pop();
    return ErrorCode::ok;
}

void TextureResourceProcessor::onLoaderTextureLoaded(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoader::TextureLoaded>(e);
    if (!ev) return;
    if (ev->id() != m_currentContentingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<TextureContented>(ev->id(), ev->texture()));
    m_currentContentingTexture = nullptr;
    auto er = contentNextTextureResource();
}

void TextureResourceProcessor::onLoaderLoadTextureFailed(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoader::LoadTextureFailed>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("texture %s load failed : %s\n", ev->id().name().c_str(), ev->error().message().c_str());
    Frameworks::EventPublisher::post(std::make_shared<ContentTextureFailed>(ev->id(), ev->error()));
    m_currentContentingTexture = nullptr;
    auto er = contentNextTextureResource();
}
