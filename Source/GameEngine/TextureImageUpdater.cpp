#include "TextureImageUpdater.h"
#include "TextureRepository.h"
#include "Frameworks/EventPublisher.h"
#include "EngineErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

TextureImageUpdater::TextureImageUpdater(TextureRepository* host)
{
    m_hostRepository = host;
    m_onResourceImageRetrieved = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnResourceImageRetrieved(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
    m_onResourceImageUpdated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnResourceImageUpdated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::TextureResourceImageUpdated), m_onResourceImageUpdated);
}

TextureImageUpdater::~TextureImageUpdater()
{
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
    m_onResourceImageRetrieved = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::TextureResourceImageUpdated), m_onResourceImageUpdated);
    m_onResourceImageUpdated = nullptr;
}

void TextureImageUpdater::RetrieveTextureImage(const std::shared_ptr<Texture>& target_tex, const std::string& name, const MathLib::Rect& image_rect)
{
    m_targetTextureName = name;
    m_targetTextureRect = image_rect;
    if ((target_tex) && target_tex->getDeviceTexture())
    {
        target_tex->getDeviceTexture()->Retrieve(m_targetTextureRect);
    }
    else
    {
        EventPublisher::post(std::make_shared<RetrieveTextureFailed>(m_targetTextureName, ErrorCode::targetTextureNotExists));
    }
}

void TextureImageUpdater::UpdateTextureImage(const std::shared_ptr<Texture>& target_tex, const std::string& name, const MathLib::Rect& image_rect, const byte_buffer& image_buff)
{
    m_targetTextureName = name;
    m_targetTextureRect = image_rect;
    if ((target_tex) && target_tex->getDeviceTexture())
    {
        target_tex->getDeviceTexture()->update(m_targetTextureRect, image_buff);
    }
    else
    {
        EventPublisher::post(std::make_shared<UpdateTextureFailed>(m_targetTextureName, ErrorCode::targetTextureNotExists));
    }
}

void TextureImageUpdater::OnResourceImageRetrieved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageRetrieved, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_targetTextureName) return;
    auto target_tex = ev->GetTargetTexture();
    if (!target_tex) return;
    EventPublisher::post(std::make_shared<TextureImageRetrieved>(m_targetTextureName, target_tex->GetRetrievedBuffer()));
}

void TextureImageUpdater::OnResourceImageUpdated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageUpdated, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_targetTextureName) return;
    auto target_tex = ev->GetTargetTexture();
    if (!target_tex) return;
    EventPublisher::post(std::make_shared<TextureImageUpdated>(m_targetTextureName));
}
