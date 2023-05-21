#include "TextureRepository.h"
#include "TextureLoader.h"
#include "Platforms/MemoryMacro.h"
#include "TextureRequests.h"
#include "TextureResponses.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "EngineErrors.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, TextureRepository, ISystemService);

TextureRepository::TextureRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager), m_currentRequestRuid()
{
    m_needTick = false;
    m_currentJob = TexturePolicy::JobType::None;
    m_loader = new TextureLoader(this);
}

TextureRepository::~TextureRepository()
{
    SAFE_DELETE(m_loader);
}

Enigma::Frameworks::ServiceResult TextureRepository::OnInit()
{
    m_onTextureLoaded =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureLoaded(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnLoadTextureFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);

    m_doLoadingTexture =
        std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestLoadTexture), m_doLoadingTexture);
    m_doCreatingTexture =
        std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestCreateTexture), m_doCreatingTexture);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult TextureRepository::OnTick()
{
    if (m_currentJob == TexturePolicy::JobType::None) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_requestsLock };
    if ((m_loadRequests.empty()) && (m_createRequests.empty()))
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_loader);
    if (!m_loadRequests.empty())
    {
        auto r = m_loadRequests.front();
        m_currentRequestRuid = r->GetRuid();
        m_loader->LoadTexture(r->GetPolicy());
        m_loadRequests.pop();
        m_currentJob = TexturePolicy::JobType::Load;
    }
    else if (!m_createRequests.empty())
    {
        auto r = m_createRequests.front();
        m_currentRequestRuid = r->GetRuid();
        m_loader->LoadTexture(r->GetPolicy());
        m_createRequests.pop();
        m_currentJob = TexturePolicy::JobType::Create;
    }
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult TextureRepository::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;

    Frameworks::RequestBus::Unsubscribe(typeid(RequestLoadTexture), m_doLoadingTexture);
    m_doLoadingTexture = nullptr;
    Frameworks::RequestBus::Unsubscribe(typeid(RequestCreateTexture), m_doCreatingTexture);
    m_doCreatingTexture = nullptr;
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

/*error TextureRepository::LoadTexture(const TexturePolicy& policy)
{
    std::lock_guard locker{ m_requestsLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}*/

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
        Frameworks::ResponseBus::Post(std::make_shared<LoadTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture(), ErrorCode::ok));
    }
    else if (m_currentJob == TexturePolicy::JobType::Create)
    {
        Frameworks::ResponseBus::Post(std::make_shared<CreateTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture(), ErrorCode::ok));
    }
    m_currentJob = TexturePolicy::JobType::None;
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
        Frameworks::ResponseBus::Post(std::make_shared<LoadTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), nullptr, ev->GetError()));
    }
    else if (m_currentJob == TexturePolicy::JobType::Create)
    {
        Frameworks::ResponseBus::Post(std::make_shared<CreateTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), nullptr, ev->GetError()));
    }
    else
    {
        assert(false);
    }
    m_currentJob = TexturePolicy::JobType::None;
}

void TextureRepository::DoLoadingTexture(const Frameworks::IRequestPtr& r)
{
    if (!r) return;
    if (auto req = std::dynamic_pointer_cast<RequestLoadTexture, Frameworks::IRequest>(r))
    {
        if (!req) return;
        std::lock_guard locker{ m_requestsLock };
        m_loadRequests.push(req);
        m_needTick = true;
    }
    else if (auto req = std::dynamic_pointer_cast<RequestCreateTexture, Frameworks::IRequest>(r))
    {
        if (!req) return;
        std::lock_guard locker{ m_requestsLock };
        m_createRequests.push(req);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}
