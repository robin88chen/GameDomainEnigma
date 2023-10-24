#include "TextureRepository.h"
#include "TextureLoader.h"
#include "TextureImageUpdater.h"
#include "Platforms/MemoryMacro.h"
#include "TextureCommands.h"
#include "TextureEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "EngineErrors.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, TextureRepository, ISystemService);

TextureRepository::TextureRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager), m_currentRequestRuid()
{
    m_needTick = false;
    m_currentJob = TexturePolicy::JobType::None;
    m_currentRequesting = false;
    m_loader = new TextureLoader(this);
    m_updater = new TextureImageUpdater(this);
}

TextureRepository::~TextureRepository()
{
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_updater);
}

Enigma::Frameworks::ServiceResult TextureRepository::onInit()
{
    m_onTextureLoaded =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureLoaded(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnLoadTextureFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);
    m_onTextureSaved =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureSaved(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::TextureSaved), m_onTextureSaved);
    m_onSaveTextureFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnSaveTextureFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::SaveTextureFailed), m_onSaveTextureFailed);

    m_onTextureImageRetrieved =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureImageRetrieved(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageUpdater::TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onRetrieveTextureImageFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnRetrieveTextureImageFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageUpdater::RetrieveTextureFailed), m_onRetrieveTextureImageFailed);
    m_onTextureImageUpdated =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureImageUpdated(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageUpdater::TextureImageUpdated), m_onTextureImageUpdated);
    m_onUpdateTextureImageFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnUpdateTextureImageFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageUpdater::UpdateTextureFailed), m_onUpdateTextureImageFailed);

    m_doLoadingTexture =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::CommandBus::Subscribe(typeid(LoadTexture), m_doLoadingTexture);
    m_doCreatingTexture =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::CommandBus::Subscribe(typeid(CreateTexture), m_doCreatingTexture);
    m_doSavingTexture =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoSavingTexture(c); });
    Frameworks::CommandBus::Subscribe(typeid(SaveTexture), m_doSavingTexture);
    m_doRetrievingTextureImage =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoRetrievingTextureImage(c); });
    Frameworks::CommandBus::Subscribe(typeid(RetrieveTextureImage), m_doRetrievingTextureImage);
    m_doUpdatingTextureImage =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoUpdatingTextureImage(c); });
    Frameworks::CommandBus::Subscribe(typeid(UpdateTextureImage), m_doUpdatingTextureImage);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult TextureRepository::onTick()
{
    if (m_currentRequesting) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_requestsLock };
    assert(m_loader);
    assert(m_updater);
    if (!m_requests.empty())
    {
        auto r = m_requests.front();
        m_currentRequestRuid = r->getRuid();
        InvokeRequest(r);
        m_requests.pop();
        m_currentRequesting = true;
    }
    else
    {
        m_needTick = false;
    }
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult TextureRepository::onTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::TextureSaved), m_onTextureSaved);
    m_onTextureSaved = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::SaveTextureFailed), m_onSaveTextureFailed);
    m_onSaveTextureFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageUpdater::TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onTextureImageRetrieved = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageUpdater::RetrieveTextureFailed), m_onRetrieveTextureImageFailed);
    m_onRetrieveTextureImageFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageUpdater::TextureImageUpdated), m_onTextureImageUpdated);
    m_onTextureImageUpdated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageUpdater::UpdateTextureFailed), m_onUpdateTextureImageFailed);
    m_onUpdateTextureImageFailed = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(LoadTexture), m_doLoadingTexture);
    m_doLoadingTexture = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(CreateTexture), m_doCreatingTexture);
    m_doCreatingTexture = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(SaveTexture), m_doSavingTexture);
    m_doSavingTexture = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(RetrieveTextureImage), m_doRetrievingTextureImage);
    m_doRetrievingTextureImage = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(UpdateTextureImage), m_doUpdatingTextureImage);
    m_doUpdatingTextureImage = nullptr;

    return Frameworks::ServiceResult::Complete;
}

bool TextureRepository::HasTexture(const std::string& name)
{
    std::lock_guard locker{ m_textureMapLock };
    auto it = m_textures.find(name);
    return ((it != m_textures.end()) && (!it->second.expired()));
}

std::shared_ptr<Texture> TextureRepository::QueryTexture(const std::string& name)
{
    std::lock_guard locker{ m_textureMapLock };
    auto it = m_textures.find(name);
    if (it == m_textures.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

void TextureRepository::OnTextureLoaded(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoader::TextureLoaded, Frameworks::IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_textureMapLock };
    m_textures.insert_or_assign(ev->GetTextureName(), ev->GetTexture());
    if (m_currentJob == TexturePolicy::JobType::Load)
    {
        Frameworks::EventPublisher::Post(std::make_shared<TextureLoaded>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture()));
    }
    else if (m_currentJob == TexturePolicy::JobType::Create)
    {
        Frameworks::EventPublisher::Post(std::make_shared<TextureCreated>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture()));
    }
    m_currentJob = TexturePolicy::JobType::None;
    m_currentRequesting = false;
}

void TextureRepository::OnLoadTextureFailed(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoader::LoadTextureFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("texture %s load failed : %s\n",
        ev->GetTextureName().c_str(), ev->GetError().message().c_str());
    if (m_currentJob == TexturePolicy::JobType::Load)
    {
        Frameworks::EventPublisher::Post(std::make_shared<LoadTextureFailed>(m_currentRequestRuid, ev->GetTextureName(), ev->GetError()));
    }
    else if (m_currentJob == TexturePolicy::JobType::Create)
    {
        Frameworks::EventPublisher::Post(std::make_shared<CreateTextureFailed>(m_currentRequestRuid, ev->GetTextureName(), ev->GetError()));
    }
    else
    {
        assert(false);
    }
    m_currentJob = TexturePolicy::JobType::None;
    m_currentRequesting = false;
}

void TextureRepository::OnTextureSaved(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureLoader::TextureSaved, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<TextureSaved>(m_currentRequestRuid, ev->GetTextureName()));
    m_currentRequesting = false;
}

void TextureRepository::OnSaveTextureFailed(const Frameworks::IEventPtr& e)
{
    assert(m_loader);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureLoader::SaveTextureFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<SaveTextureFailed>(m_currentRequestRuid, ev->GetTextureName(), ev->GetError()));
    m_currentRequesting = false;
}

void TextureRepository::OnTextureImageRetrieved(const Frameworks::IEventPtr& e)
{
    assert(m_updater);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageUpdater::TextureImageRetrieved, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<TextureImageRetrieved>(m_currentRequestRuid, ev->GetTextureName(), ev->GetImageBuffer()));
    m_currentRequesting = false;
}

void TextureRepository::OnRetrieveTextureImageFailed(const Frameworks::IEventPtr& e)
{
    assert(m_updater);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageUpdater::RetrieveTextureFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<RetrieveTextureImageFailed>(m_currentRequestRuid, ev->GetTextureName(), ev->GetError()));
    m_currentRequesting = false;
}

void TextureRepository::OnTextureImageUpdated(const Frameworks::IEventPtr& e)
{
    assert(m_updater);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageUpdater::TextureImageUpdated, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<TextureImageUpdated>(m_currentRequestRuid, ev->GetTextureName()));
    m_currentRequesting = false;
}

void TextureRepository::OnUpdateTextureImageFailed(const Frameworks::IEventPtr& e)
{
    assert(m_updater);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageUpdater::UpdateTextureFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<UpdateTextureImageFailed>(m_currentRequestRuid, ev->GetTextureName(), ev->GetError()));
    m_currentRequesting = false;
}

void TextureRepository::DoLoadingTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (auto cmd = std::dynamic_pointer_cast<LoadTexture>(c))
    {
        if (!cmd) return;
        std::lock_guard locker{ m_requestsLock };
        m_requests.push(cmd);
        m_needTick = true;
    }
    else if (auto cmd = std::dynamic_pointer_cast<CreateTexture>(c))
    {
        if (!cmd) return;
        std::lock_guard locker{ m_requestsLock };
        m_requests.push(cmd);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}

void TextureRepository::DoSavingTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (auto cmd = std::dynamic_pointer_cast<SaveTexture>(c))
    {
        std::lock_guard locker{ m_requestsLock };
        m_requests.push(cmd);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}

void TextureRepository::DoRetrievingTextureImage(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (auto cmd = std::dynamic_pointer_cast<RetrieveTextureImage>(c))
    {
        std::lock_guard locker{ m_requestsLock };
        m_requests.push(cmd);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}

void TextureRepository::DoUpdatingTextureImage(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (auto cmd = std::dynamic_pointer_cast<UpdateTextureImage>(c))
    {
        std::lock_guard locker{ m_requestsLock };
        m_requests.push(cmd);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}

void TextureRepository::InvokeRequest(const std::shared_ptr<Frameworks::IRequestCommand>& request)
{
    assert(m_loader);
    assert(m_updater);
    if (auto load_request = std::dynamic_pointer_cast<LoadTexture>(request))
    {
        m_loader->LoadTexture(load_request->GetPolicy());
        m_currentJob = TexturePolicy::JobType::Load;
    }
    else if (auto create_request = std::dynamic_pointer_cast<CreateTexture>(request))
    {
        m_loader->LoadTexture(create_request->GetPolicy());
        m_currentJob = TexturePolicy::JobType::Create;
    }
    else if (auto save_request = std::dynamic_pointer_cast<SaveTexture>(request))
    {
        m_loader->SaveTexture(save_request->GetTargetTexture(), save_request->GetTextureName(), save_request->GetFile());
    }
    else if (auto retrieve_request = std::dynamic_pointer_cast<RetrieveTextureImage>(request))
    {
        m_updater->RetrieveTextureImage(retrieve_request->GetTargetTexture(), retrieve_request->GetTextureName(), retrieve_request->GetImageRect());
    }
    else if (auto update_request = std::dynamic_pointer_cast<UpdateTextureImage>(request))
    {
        m_updater->UpdateTextureImage(update_request->GetTargetTexture(), update_request->GetTextureName(), update_request->GetImageRect(), update_request->GetImageBuffer());
    }
    else
    {
        assert(false);
    }
}
