#include "TextureRepository.h"
#include "TextureLoader.h"
#include "TextureImageRetriever.h"
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
    m_currentRequesting = false;
    m_loader = new TextureLoader(this);
    m_retriever = new TextureImageRetriever(this);
}

TextureRepository::~TextureRepository()
{
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_retriever);
}

Enigma::Frameworks::ServiceResult TextureRepository::OnInit()
{
    m_onTextureLoaded =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureLoaded(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnLoadTextureFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);
    m_onTextureImageRetrieved =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnTextureImageRetrieved(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageRetriever::TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onRetrieveTextureImageFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnRetrieveTextureImageFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(TextureImageRetriever::RetrieveTextureFailed), m_onRetrieveTextureImageFailed);

    m_doLoadingTexture =
        std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestLoadTexture), m_doLoadingTexture);
    m_doCreatingTexture =
        std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoLoadingTexture(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestCreateTexture), m_doCreatingTexture);
    m_doRetrievingTextureImage =
        std::make_shared<Frameworks::RequestSubscriber>([=](auto c) { this->DoRetrievingTextureImage(c); });
    Frameworks::RequestBus::Subscribe(typeid(RequestRetrieveTextureImage), m_doRetrievingTextureImage);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult TextureRepository::OnTick()
{
    if (m_currentRequesting) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_requestsLock };
    assert(m_loader);
    assert(m_retriever);
    if (!m_loadRequests.empty())
    {
        auto r = m_loadRequests.front();
        m_currentRequestRuid = r->GetRuid();
        m_loader->LoadTexture(r->GetPolicy());
        m_loadRequests.pop();
        m_currentJob = TexturePolicy::JobType::Load;
        m_currentRequesting = true;
    }
    else if (!m_createRequests.empty())
    {
        auto r = m_createRequests.front();
        m_currentRequestRuid = r->GetRuid();
        m_loader->LoadTexture(r->GetPolicy());
        m_createRequests.pop();
        m_currentJob = TexturePolicy::JobType::Create;
        m_currentRequesting = true;
    }
    else if (!m_retrieveRequests.empty())
    {
        auto r = m_retrieveRequests.front();
        m_currentRequestRuid = r->GetRuid();
        m_retriever->RetrieveTextureImage(r->GetTextureName(), r->GetImageRect());
        m_retrieveRequests.pop();
        m_currentRequesting = true;
    }
    else
    {
        m_needTick = false;
    }
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult TextureRepository::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureLoader::LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageRetriever::TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onTextureImageRetrieved = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(TextureImageRetriever::RetrieveTextureFailed), m_onRetrieveTextureImageFailed);
    m_onRetrieveTextureImageFailed = nullptr;

    Frameworks::RequestBus::Unsubscribe(typeid(RequestLoadTexture), m_doLoadingTexture);
    m_doLoadingTexture = nullptr;
    Frameworks::RequestBus::Unsubscribe(typeid(RequestCreateTexture), m_doCreatingTexture);
    m_doCreatingTexture = nullptr;
    Frameworks::RequestBus::Unsubscribe(typeid(RequestRetrieveTextureImage), m_doRetrievingTextureImage);
    m_doRetrievingTextureImage = nullptr;

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
        Frameworks::ResponseBus::Post(std::make_shared<LoadTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture(), ErrorCode::ok));
    }
    else if (m_currentJob == TexturePolicy::JobType::Create)
    {
        Frameworks::ResponseBus::Post(std::make_shared<CreateTextureResponse>(m_currentRequestRuid, ev->GetTextureName(), ev->GetTexture(), ErrorCode::ok));
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
    m_currentRequesting = false;
}

void TextureRepository::OnTextureImageRetrieved(const Frameworks::IEventPtr& e)
{
    assert(m_retriever);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageRetriever::TextureImageRetrieved, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::ResponseBus::Post(std::make_shared<RetrieveTextureImageResponse>(m_currentRequestRuid, ev->GetTextureName(), ev->GetImageBuffer(), ErrorCode::ok));
    m_currentRequesting = false;
}

void TextureRepository::OnRetrieveTextureImageFailed(const Frameworks::IEventPtr& e)
{
    assert(m_retriever);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureImageRetriever::RetrieveTextureFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::ResponseBus::Post(std::make_shared<RetrieveTextureImageResponse>(m_currentRequestRuid, ev->GetTextureName(), byte_buffer{}, ev->GetError()));
    m_currentRequesting = false;
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

void TextureRepository::DoRetrievingTextureImage(const Frameworks::IRequestPtr& r)
{
    if (!r) return;
    if (auto req = std::dynamic_pointer_cast<RequestRetrieveTextureImage, Frameworks::IRequest>(r))
    {
        std::lock_guard locker{ m_requestsLock };
        m_retrieveRequests.push(req);
        m_needTick = true;
    }
    else
    {
        assert(false);
    }
}

