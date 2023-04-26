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
    if (m_hostRepository->HasTexture(m_policy.m_name))
    {
        Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(
            m_policy.m_name, m_hostRepository->QueryTexture(m_policy.m_name)));
    }
    else
    {
        Frameworks::CommandBus::Post(std::make_shared<Graphics::CreateTexture>(m_policy.m_name));
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
            m_policy.m_name, ErrorCode::findStashedAssetFail));
        return;
    }
    texture.value()->Load(m_policy.m_filename, m_policy.m_pathId);
}

void TextureLoader::OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageLoaded, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.m_name) return;
    Graphics::ITexturePtr dev_tex = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::ITexturePtr>(m_policy.m_name);
    if (!dev_tex)
    {
        Platforms::Debug::Printf("can't get texture asset %s", ev->GetTextureName().c_str());
        Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
            m_policy.m_name, ErrorCode::findStashedAssetFail));
        return;
    }
    auto tex = std::make_shared<Texture>(m_policy.m_name, dev_tex);
    tex->TheFactoryDesc().ClaimAsResourceAsset(m_policy.m_name, m_policy.m_filename, m_policy.m_pathId);
    Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(m_policy.m_name, tex));
}

void TextureLoader::OnTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceLoadImageFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.m_name) return;
    Platforms::Debug::Printf("texture %s load image %s failed", m_policy.m_name.c_str(), m_policy.m_filename.c_str());
    Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
        m_policy.m_name, ev->GetError()));
}
