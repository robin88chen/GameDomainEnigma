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
#include "GraphicKernel/IMultiTexture.h"
#include <memory>

using namespace Enigma::Engine;

TextureLoader::TextureLoader(TextureRepository* host)
{
    m_hostRepository = host;

    m_onTextureCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onMultiTextureCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceMultiTextureCreated), m_onMultiTextureCreated);
    m_onTextureImageLoaded = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureImageLoaded(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureLoadImageFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureLoadImageFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
    m_onTextureResourceCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureResourceCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceFromMemoryCreated), m_onTextureResourceCreated);
    m_onMultiTextureResourceCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureResourceCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::MultiTextureResourceFromMemoryCreated), m_onMultiTextureResourceCreated);
    m_onTextureCreateResourceFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureCreateResourceFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceCreateFromMemoryFailed), m_onTextureCreateResourceFailed);
    m_onTextureImageSaved = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureImageSaved(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceImageSaved), m_onTextureImageSaved);
    m_onTextureSaveImageFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnTextureSaveImageFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::TextureResourceSaveImageFailed), m_onTextureSaveImageFailed);
}

TextureLoader::~TextureLoader()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceTextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceMultiTextureCreated), m_onMultiTextureCreated);
    m_onMultiTextureCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceImageLoaded), m_onTextureImageLoaded);
    m_onTextureImageLoaded = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceLoadImageFailed), m_onTextureLoadImageFailed);
    m_onTextureLoadImageFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceFromMemoryCreated), m_onTextureResourceCreated);
    m_onTextureResourceCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::MultiTextureResourceFromMemoryCreated), m_onMultiTextureResourceCreated);
    m_onMultiTextureResourceCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceCreateFromMemoryFailed), m_onTextureCreateResourceFailed);
    m_onTextureCreateResourceFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceImageSaved), m_onTextureImageSaved);
    m_onTextureImageSaved = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::TextureResourceSaveImageFailed), m_onTextureSaveImageFailed);
    m_onTextureSaveImageFailed = nullptr;
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
        if (m_policy.GetSurfaceCount() > 1)
        {
            Frameworks::CommandBus::Post(std::make_shared<Graphics::CreateMultiTexture>(m_policy.GetName()));
        }
        else
        {
            Frameworks::CommandBus::Post(std::make_shared<Graphics::CreateTexture>(m_policy.GetName()));
        }
    }
}

void TextureLoader::SaveTexture(const std::shared_ptr<Texture>& target_texture, const std::string& name, const FileSystem::IFilePtr& file)
{
    assert(file);
    if (!target_texture)
    {
        Frameworks::EventPublisher::Post(std::make_shared<SaveTextureFailed>(name, ErrorCode::targetTextureNotExists));
        return;
    }
    if (!target_texture->GetDeviceTexture())
    {
        Frameworks::EventPublisher::Post(std::make_shared<SaveTextureFailed>(name, ErrorCode::targetTextureNotExists));
        return;
    }
    m_savingTextureName = name;
    m_savingDeviceTextureName = target_texture->GetDeviceTexture()->GetName();
    target_texture->GetDeviceTexture()->Save(file);
}

void TextureLoader::OnTextureCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::string tex_name;
    if (auto ev = std::dynamic_pointer_cast<Graphics::DeviceTextureCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->GetTextureName();
    }
    else if (auto ev = std::dynamic_pointer_cast<Graphics::DeviceMultiTextureCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->GetTextureName();
    }
    else return;
    auto texture = Graphics::IGraphicAPI::Instance()
        ->TryFindGraphicAsset<Graphics::ITexturePtr>(tex_name);
    if (!texture)
    {
        Platforms::Debug::Printf("can't get texture asset %s", tex_name.c_str());
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
        if (m_policy.GetSurfaceCount() > 1)
        {
            std::vector<byte_buffer> buffers;
            buffers.resize(m_policy.GetSurfaceCount());
            std::dynamic_pointer_cast<Graphics::IMultiTexture, Graphics::ITexture>(texture.value())->MultiCreate(m_policy.GetDimension(), m_policy.GetSurfaceCount(), buffers);
        }
        else
        {
            texture.value()->Create(m_policy.GetDimension(), byte_buffer{});
        }
    }
}

void TextureLoader::OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageLoaded, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.GetName()) return;
    if (m_policy.GetJobType() != TexturePolicy::JobType::Load) return;
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

void TextureLoader::OnTextureResourceCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    std::string tex_name;
    if (auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceFromMemoryCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->GetTextureName();
    }
    else if (auto ev = std::dynamic_pointer_cast<Graphics::MultiTextureResourceFromMemoryCreated, Frameworks::IEvent>(e))
    {
        tex_name = ev->GetTextureName();
    }
    else return;
    if (tex_name != m_policy.GetName()) return;
    if (m_policy.GetJobType() != TexturePolicy::JobType::Create) return;
    Graphics::ITexturePtr dev_tex = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::ITexturePtr>(m_policy.GetName());
    if (!dev_tex)
    {
        Platforms::Debug::Printf("can't get texture asset %s", tex_name.c_str());
        Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(
            m_policy.GetName(), ErrorCode::findStashedAssetFail));
        return;
    }
    auto tex = std::make_shared<Texture>(m_policy.GetName(), dev_tex);
    Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(m_policy.GetName(), tex));
}

void TextureLoader::OnTextureCreateResourceFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceCreateFromMemoryFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_policy.GetName()) return;
    Platforms::Debug::Printf("texture %s create from memory failed", m_policy.GetName().c_str());
    Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(m_policy.GetName(), ev->GetError()));
}

void TextureLoader::OnTextureImageSaved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceImageSaved, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_savingDeviceTextureName) return;
    Frameworks::EventPublisher::Post(std::make_shared<TextureSaved>(m_savingTextureName));
}

void TextureLoader::OnTextureSaveImageFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::TextureResourceSaveImageFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != m_savingDeviceTextureName) return;
    Frameworks::EventPublisher::Post(std::make_shared<SaveTextureFailed>(m_savingTextureName, ev->GetError()));
}
