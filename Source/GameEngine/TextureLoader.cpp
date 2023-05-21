#include "TextureLoader.h"
#include "EngineErrors.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/ITexture.h"
#include "Platforms/PlatformLayer.h"
#include "TextureRepository.h"
#include <memory>

using namespace Enigma::Engine;

TextureLoader::TextureLoader(TextureRepository* host)
{
    m_hostRepository = host;

    m_onTextureCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onTextureImageLoaded = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureImageLoaded(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureLoadImageFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureLoadImageFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
}

TextureLoader::~TextureLoader()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureImageLoaded = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
    m_onTextureLoadImageFailed = nullptr;
}

void TextureLoader::LoadTexture(const TexturePolicy& policy)
{
    m_policy = policy;
    if (m_hostRepository->HasTexture(m_policy.GetName()))
    {
        Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(
            m_policy.GetName(), m_hostRepository->QueryTexture(m_policy.GetName())));
    }
    else
    {
        Frameworks::CommandBus::Post(std::make_shared<Graphics::CreateTexture>(m_policy.GetName()));
    }
}

void TextureLoader::OnTextureCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceTextureCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    auto texture = Graphics::IGraphicAPI::Instance()
        ->TryFindGraphicAsset<Graphics::ITexturePtr>(ev->GetTextureName());
    if (!texture)
    {
        Platforms::Debug::Printf("can't get texture asset %s", ev->GetTextureName().c_str());
        Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
            m_policy.GetName(), ErrorCode::findStashedAssetFail));
        return;
    }
    if (m_policy.GetJobType() == TexturePolicy::JobType::Load)
    {
        texture.value()->Load(m_policy.GetFileName(), m_policy.GetPathID());
    }
    else
    {
        texture.value()->Create(m_policy.GetDimension(), byte_buffer{});
    }
}

void TextureLoader::OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageLoaded, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.GetName()) return;
    Graphics::ITexturePtr dev_tex = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::ITexturePtr>(m_policy.GetName());
    if (!dev_tex)
    {
        Platforms::Debug::Printf("can't get texture asset %s", ev->GetTextureName().c_str());
        Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
            m_policy.GetName(), ErrorCode::findStashedAssetFail));
        return;
    }
    auto tex = std::make_shared<Texture>(m_policy.GetName(), dev_tex);
    tex->TheFactoryDesc().ClaimAsResourceAsset(m_policy.GetName(), m_policy.GetFileName(), m_policy.GetPathID());
    Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(m_policy.GetName(), tex));
}

void TextureLoader::OnTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceLoadImageFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.GetName()) return;
    Platforms::Debug::Printf("texture %s load image %s failed", m_policy.GetName().c_str(), m_policy.GetFileName().c_str());
    Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
        m_policy.GetName(), ev->GetError()));
}
