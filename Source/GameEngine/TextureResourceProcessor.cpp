#include "TextureResourceProcessor.h"
#include "TextureLoader.h"
#include "TextureSaver.h"
#include "TextureImageUpdater.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "TextureDto.h"
#include "EngineErrors.h"
#include "TextureEvents.h"
#include "TextureCommands.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Engine;

TextureResourceProcessor::TextureResourceProcessor()
{
    m_loader = menew TextureLoader();
    m_saver = menew TextureSaver();
    m_imageUpdater = menew TextureImageUpdater();
    registerHandlers();
}

TextureResourceProcessor::~TextureResourceProcessor()
{
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_saver);
    SAFE_DELETE(m_imageUpdater);
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
    m_enqueueSavingTexture = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { enqueueSavingTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(EnqueueSavingTexture), m_enqueueSavingTexture);
    m_enqueueRetrievingImage = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { enqueueRetrievingImage(c); });
    Frameworks::CommandBus::subscribe(typeid(EnqueueRetrievingTextureImage), m_enqueueRetrievingImage);
    m_enqueueUpdatingImage = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { enqueueUpdatingImage(c); });
    Frameworks::CommandBus::subscribe(typeid(EnqueueUpdatingTextureImage), m_enqueueUpdatingImage);
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
    Frameworks::CommandBus::unsubscribe(typeid(EnqueueSavingTexture), m_enqueueSavingTexture);
    m_enqueueSavingTexture = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(EnqueueRetrievingTextureImage), m_enqueueRetrievingImage);
    m_enqueueRetrievingImage = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(EnqueueUpdatingTextureImage), m_enqueueUpdatingImage);
    m_enqueueUpdatingImage = nullptr;
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

std::error_code TextureResourceProcessor::enqueueRetrievingTexture(const std::shared_ptr<Texture>& texture, const MathLib::Rect& rect)
{
    assert(texture);
    if (!texture->lazyStatus().isReady()) return ErrorCode::textureNotReady;
    std::lock_guard locker{ m_retrievingQueueLock };
    m_retrievingQueue.push({ texture, rect });
    return ErrorCode::ok;
}

std::error_code TextureResourceProcessor::enqueueUpdatingTexture(const std::shared_ptr<Texture>& texture, const MathLib::Rect& rect, const byte_buffer& buff)
{
    assert(texture);
    if (!texture->lazyStatus().isReady()) return ErrorCode::textureNotReady;
    std::lock_guard locker{ m_updatingQueueLock };
    m_updatingQueue.push({ texture, rect, buff });
    return ErrorCode::ok;
}

std::error_code TextureResourceProcessor::processNextTextureResource()
{
    if (m_currentProcessingTexture) return ErrorCode::ok;
    assert(m_imageUpdater);
    std::lock_guard retrieve_locker{ m_retrievingQueueLock };
    std::lock_guard update_locker{ m_updatingQueueLock };
    if ((m_retrievingQueue.empty()) && (m_updatingQueue.empty()))
    {
        m_currentProcessingTexture = nullptr;
        return ErrorCode::ok;
    }
    if (!m_retrievingQueue.empty())
    {
        auto& [texture, rect] = m_retrievingQueue.front();
        m_currentProcessingTexture = texture;
        m_imageUpdater->retrieveTextureImage(texture, rect);
        m_retrievingQueue.pop();
        return ErrorCode::ok;
    }
    else if (!m_updatingQueue.empty())
    {
        auto& [texture, rect, buff] = m_updatingQueue.front();
        m_currentProcessingTexture = texture;
        m_imageUpdater->updateTextureImage(texture, rect, buff);
        m_updatingQueue.pop();
        return ErrorCode::ok;
    }
    else
        assert(false);
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
    const auto er = contentNextTextureResource();
    assert(!er);
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
    const auto er = contentNextTextureResource();
    assert(!er);
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
    const auto er = saveNextTextureResource();
    assert(!er);
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
    const auto er = saveNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::onUpdaterImageRetrieved(const Frameworks::IEventPtr& e)
{
    assert(m_imageUpdater);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageUpdater::TextureImageRetrieved>(e);
    if (!ev) return;
    if (ev->id() != m_currentProcessingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<TextureImageRetrieved>(ev->id(), ev->buffer()));
    m_currentProcessingTexture = nullptr;
    const auto er = processNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::onUpdaterRetrieveImageFailed(const Frameworks::IEventPtr& e)
{
    assert(m_imageUpdater);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageUpdater::RetrieveTextureFailed>(e);
    if (!ev) return;
    if (ev->id() != m_currentProcessingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<RetrieveTextureImageFailed>(ev->id(), ev->error()));
    m_currentProcessingTexture = nullptr;
    const auto er = processNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::onUpdaterImageUpdated(const Frameworks::IEventPtr& e)
{
    assert(m_imageUpdater);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageUpdater::TextureImageUpdated>(e);
    if (!ev) return;
    if (ev->id() != m_currentProcessingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<TextureImageUpdated>(ev->id()));
    m_currentProcessingTexture = nullptr;
    const auto er = processNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::onUpdaterUpdateImageFailed(const Frameworks::IEventPtr& e)
{
    assert(m_imageUpdater);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageUpdater::UpdateTextureFailed>(e);
    if (!ev) return;
    if (ev->id() != m_currentProcessingTexture->id()) return;
    Frameworks::EventPublisher::post(std::make_shared<UpdateTextureImageFailed>(ev->id(), ev->error()));
    m_currentProcessingTexture = nullptr;
    const auto er = processNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::enqueueSavingTexture(const Frameworks::ICommandPtr& c)
{
    assert(c);
    const auto command = std::dynamic_pointer_cast<EnqueueSavingTexture>(c);
    assert(command);
    auto er = enqueueSavingTexture(command->targetTexture(), command->file());
    assert(!er);
    er = saveNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::enqueueRetrievingImage(const Frameworks::ICommandPtr& c)
{
    assert(c);
    const auto command = std::dynamic_pointer_cast<EnqueueRetrievingTextureImage>(c);
    assert(command);
    auto er = enqueueRetrievingTexture(command->targetTexture(), command->imageRect());
    assert(!er);
    er = processNextTextureResource();
    assert(!er);
}

void TextureResourceProcessor::enqueueUpdatingImage(const Frameworks::ICommandPtr& c)
{
    assert(c);
    const auto command = std::dynamic_pointer_cast<EnqueueUpdatingTextureImage>(c);
    assert(command);
    auto er = enqueueUpdatingTexture(command->targetTexture(), command->imageRect(), command->imageBuffer());
    assert(!er);
    er = processNextTextureResource();
    assert(!er);
}
