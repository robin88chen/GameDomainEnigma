#include "TextureImageUpdater.h"
#include "TextureRepository.h"
#include "Frameworks/EventPublisher.h"
#include "EngineErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

TextureImageUpdater::TextureImageUpdater()
{
    m_onResourceImageRetrieved = std::make_shared<EventSubscriber>([=](auto e) { this->onResourceImageRetrieved(e); });
    EventPublisher::subscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
    m_onResourceImageUpdated = std::make_shared<EventSubscriber>([=](auto e) { this->onResourceImageUpdated(e); });
    EventPublisher::subscribe(typeid(Graphics::TextureResourceImageUpdated), m_onResourceImageUpdated);
}

TextureImageUpdater::~TextureImageUpdater()
{
    EventPublisher::unsubscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
    m_onResourceImageRetrieved = nullptr;
    EventPublisher::unsubscribe(typeid(Graphics::TextureResourceImageUpdated), m_onResourceImageUpdated);
    m_onResourceImageUpdated = nullptr;
}

void TextureImageUpdater::retrieveTextureImage(const std::shared_ptr<Texture>& target_tex, const MathLib::Rect& image_rect)
{
    assert(target_tex);
    m_targetTexture = target_tex;
    m_targetTextureRect = image_rect;
    if ((target_tex) && target_tex->getDeviceTexture())
    {
        target_tex->getDeviceTexture()->Retrieve(m_targetTextureRect);
    }
    else
    {
        EventPublisher::post(std::make_shared<RetrieveTextureFailed>(m_targetTexture->id(), ErrorCode::targetTextureNotExists));
    }
}

void TextureImageUpdater::updateTextureImage(const std::shared_ptr<Texture>& target_tex, const MathLib::Rect& image_rect, const byte_buffer& image_buff)
{
    assert(target_tex);
    m_targetTexture = target_tex;
    m_targetTextureRect = image_rect;
    if ((target_tex) && target_tex->getDeviceTexture())
    {
        target_tex->getDeviceTexture()->update(m_targetTextureRect, image_buff);
    }
    else
    {
        EventPublisher::post(std::make_shared<UpdateTextureFailed>(m_targetTexture->id(), ErrorCode::targetTextureNotExists));
    }
}

void TextureImageUpdater::onResourceImageRetrieved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageRetrieved>(e);
    if (!ev) return;
    if (ev->textureName() != m_targetTexture->getDeviceTexture()->getName()) return;
    auto target_tex = ev->targetDeviceTexture();
    if (!target_tex) return;
    EventPublisher::post(std::make_shared<TextureImageRetrieved>(m_targetTexture->id(), target_tex->GetRetrievedBuffer()));
}

void TextureImageUpdater::onRetrieveResourceImageFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceRetrieveImageFailed>(e);
    if (!ev) return;
    if (ev->textureName() != m_targetTexture->getDeviceTexture()->getName()) return;
    EventPublisher::post(std::make_shared<RetrieveTextureFailed>(m_targetTexture->id(), ev->error()));
}

void TextureImageUpdater::onResourceImageUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageUpdated>(e);
    if (!ev) return;
    if (ev->textureName() != m_targetTexture->getDeviceTexture()->getName()) return;
    auto target_tex = ev->targetDeviceTexture();
    if (!target_tex) return;
    EventPublisher::post(std::make_shared<TextureImageUpdated>(m_targetTexture->id()));
}

void TextureImageUpdater::onUpdateResourceImageFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceUpdateImageFailed>(e);
    if (!ev) return;
    if (ev->textureName() != m_targetTexture->getDeviceTexture()->getName()) return;
    EventPublisher::post(std::make_shared<UpdateTextureFailed>(m_targetTexture->id(), ev->error()));
}
