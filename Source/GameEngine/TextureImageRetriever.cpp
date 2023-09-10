#include "TextureImageRetriever.h"

#include <memory>

#include "TextureRepository.h"
#include "Frameworks/EventPublisher.h"
#include "EngineErrors.h"
#include "GraphicKernel/GraphicEvents.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

TextureImageRetriever::TextureImageRetriever(TextureRepository* host)
{
    m_hostRepository = host;
    m_onResourceImageRetrieved = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnResourceImageRetrieved(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
}

TextureImageRetriever::~TextureImageRetriever()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceImageRetrieved), m_onResourceImageRetrieved);
    m_onResourceImageRetrieved = nullptr;
}

void TextureImageRetriever::RetrieveTextureImage(const std::string& name, const MathLib::Rect& image_rect)
{
    m_retrievingTextureName = name;
    m_retrievingTextureRect = image_rect;
    auto target_tex = m_hostRepository->QueryTexture(m_retrievingTextureName);
    if ((target_tex) && target_tex->GetDeviceTexture())
    {
        target_tex->GetDeviceTexture()->Retrieve(m_retrievingTextureRect);
    }
    else
    {
        EventPublisher::Post(std::make_shared<RetrieveTextureFailed>(m_retrievingTextureName, ErrorCode::targetTextureNotExists));
    }
}

void TextureImageRetriever::OnResourceImageRetrieved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageRetrieved, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_retrievingTextureName) return;
    auto target_tex = m_hostRepository->QueryTexture(m_retrievingTextureName);
    if ((!target_tex) || (!target_tex->GetDeviceTexture())) return;
    EventPublisher::Post(std::make_shared<TextureImageRetrieved>(m_retrievingTextureName, target_tex->GetDeviceTexture()->GetRetrievedBuffer()));
}
