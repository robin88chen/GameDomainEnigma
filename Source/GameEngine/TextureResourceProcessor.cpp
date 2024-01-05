#include "TextureResourceProcessor.h"
#include "TextureLoader.h"
#include "TextureSaver.h"
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
    m_saver = menew TextureSaver();
    registerHandlers();
}

TextureResourceProcessor::~TextureResourceProcessor()
{
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_saver);
    unregisterHandlers();
}

void TextureResourceProcessor::registerHandlers()
{
    m_onLoaderTextureLoaded = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onLoaderTextureLoaded(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureLoader::TextureLoaded), m_onLoaderTextureLoaded);
    m_onLoaderLoadTextureFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onLoaderLoadTextureFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoaderLoadTextureFailed);
    m_onSaverTextureSaved = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onSaverTextureSaved(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureSaver::TextureSaved), m_onSaverTextureSaved);
    m_onSaverSaveTextureFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onSaverSaveTextureFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(TextureSaver::SaveTextureFailed), m_onSaverSaveTextureFailed);
}

void TextureResourceProcessor::unregisterHandlers()
{
    Frameworks::EventPublisher::unsubscribe(typeid(TextureLoader::TextureLoaded), m_onLoaderTextureLoaded);
    m_onLoaderTextureLoaded = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoaderLoadTextureFailed);
    m_onLoaderLoadTextureFailed = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(TextureSaver::TextureSaved), m_onSaverTextureSaved);
    m_onSaverTextureSaved = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(TextureSaver::SaveTextureFailed), m_onSaverSaveTextureFailed);
    m_onSaverSaveTextureFailed = nullptr;
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

std::error_code TextureResourceProcessor::enqueueSavingTexture(const std::shared_ptr<Texture>& texture, const std::shared_ptr<FileSystem::IFile>& file)
{
    assert(texture);
    assert(file);
    if (!texture->lazyStatus().isReady()) return ErrorCode::textureNotReady;
    std::lock_guard locker{ m_savingQueueLock };
    m_savingQueue.push({ texture, file });
    return ErrorCode::ok;
}

std::error_code TextureResourceProcessor::saveNextTextureResource()
{
    if (m_currentSavingTexture) return ErrorCode::ok;
    assert(m_saver);
    std::lock_guard locker{ m_savingQueueLock };
    if (m_savingQueue.empty())
    {
        m_currentSavingTexture = nullptr;
        return ErrorCode::ok;
    }
    auto& [texture, file] = m_savingQueue.front();
    m_currentSavingTexture = texture;
    m_saver->saveTextureImage(texture, file);
    m_savingQueue.pop();
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

void TextureResourceProcessor::onSaverTextureSaved(const Frameworks::IEventPtr& e)
{
    assert(m_saver);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureSaver::TextureSaved>(e);
    if (!ev) return;
    if (ev->id() != m_currentSavingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<TextureSaved>(ev->id()));
    m_currentSavingTexture = nullptr;
    auto er = saveNextTextureResource();
}

void TextureResourceProcessor::onSaverSaveTextureFailed(const Frameworks::IEventPtr& e)
{
    assert(m_saver);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureSaver::SaveTextureFailed>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("texture %s save failed : %s\n", ev->id().name().c_str(), ev->error().message().c_str());
    Frameworks::EventPublisher::post(std::make_shared<SaveTextureFailed>(ev->id(), ev->error()));
    m_currentSavingTexture = nullptr;
    auto er = saveNextTextureResource();
}
