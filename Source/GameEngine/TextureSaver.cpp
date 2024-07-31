#include "TextureSaver.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include "EngineErrors.h"
#include "Texture.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;

TextureSaver::TextureSaver()
{
    m_onTextureImageSaved = std::make_shared<EventSubscriber>([=](const IEventPtr& e) { onTextureImageSaved(e); });
    EventPublisher::subscribe(typeid(TextureResourceImageSaved), m_onTextureImageSaved);
    m_onSaveTextureImageFailed = std::make_shared<EventSubscriber>([=](const IEventPtr& e) { onSaveTextureImageFailed(e); });
    EventPublisher::subscribe(typeid(TextureResourceSaveImageFailed), m_onSaveTextureImageFailed);
}

TextureSaver::~TextureSaver()
{
    EventPublisher::unsubscribe(typeid(TextureResourceImageSaved), m_onTextureImageSaved);
    m_onTextureImageSaved = nullptr;
    EventPublisher::unsubscribe(typeid(TextureResourceSaveImageFailed), m_onSaveTextureImageFailed);
    m_onSaveTextureImageFailed = nullptr;
}

void TextureSaver::saveTextureImage(const std::shared_ptr<Texture>& target_texture, const FileSystem::IFilePtr& file)
{
    assert(target_texture);
    assert(file);
    if (!target_texture->getDeviceTexture())
    {
        failSavingImage(target_texture->id(), ErrorCode::targetTextureNotExists);
        return;
    }
    m_savingTexture = target_texture;
    target_texture->getDeviceTexture()->save(file);
}

void TextureSaver::onTextureImageSaved(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageSaved, Frameworks::IEvent>(e);
    if (!ev) return;
    if (!m_savingTexture) return;
    if (ev->textureName() != m_savingTexture->getDeviceTexture()->getName()) return;
    Frameworks::EventPublisher::enqueue(std::make_shared<TextureSaved>(m_savingTexture->id()));
}

void TextureSaver::onSaveTextureImageFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceSaveImageFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (!m_savingTexture) return;
    if (ev->textureName() != m_savingTexture->getDeviceTexture()->getName()) return;
    failSavingImage(m_savingTexture->id(), ev->error());
}

void TextureSaver::failSavingImage(const TextureId& id, std::error_code er)
{
    Frameworks::EventPublisher::enqueue(std::make_shared<SaveTextureFailed>(id, er));
}

