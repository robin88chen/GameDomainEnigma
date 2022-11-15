#include "RequestBus.h"
#include <cassert>

using namespace Enigma::Frameworks;

DEFINE_RTTI(Frameworks, RequestBus, ISystemService);

RequestBus* RequestBus::m_thisBus = nullptr;

RequestBus::RequestBus(ServiceManager* manager) : ISystemService(manager)
{
    assert(m_thisBus == nullptr);
    m_needTick = false;
    m_orderValue = MessageServiceOrderValue;
    m_thisBus = this;
}

RequestBus::~RequestBus()
{
    m_thisBus = nullptr;
}

ServiceResult RequestBus::OnTick()
{
    assert(m_thisBus);

    unsigned int req_count = 0;
    {
        std::lock_guard locker{ m_requestListLock };
        req_count = static_cast<unsigned int>(m_requests.size());
    }
    if (req_count == 0)
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    unsigned int req_sended = 0;
    while (req_sended < req_count)
    {
        IRequestPtr req = nullptr;
        {
            std::lock_guard locker{ m_requestListLock };
            if (!m_requests.empty())
            {
                req = m_requests.front();
                m_requests.pop_front();
            }
        }
        if (!req) break;
        Send(req);
        req_sended++;
    }
    return ServiceResult::Pendding;
}

ServiceResult RequestBus::OnTerm()
{
    CleanupAllRequests();

    return ServiceResult::Complete;
}

void RequestBus::Subscribe(const std::type_info& req_type, const RequestSubscriberPtr& sub)
{
    assert(m_thisBus);
    m_thisBus->m_subscribers[std::type_index{ req_type }].emplace_back(sub);
}

void RequestBus::Unsubscribe(const std::type_info& req_type, const RequestSubscriberPtr& sub)
{
    assert(m_thisBus);
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ req_type });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    subscribers->second.remove(sub);
}

void RequestBus::CleanupAllRequests()
{
    std::lock_guard locker{ m_requestListLock };
    m_requests.clear();
}

void RequestBus::Post(const IRequestPtr& r)
{
    assert(m_thisBus);
    if (!r) return;
    if (m_thisBus->m_isSuspended) return;

    std::lock_guard locker{ m_thisBus->m_requestListLock };
    m_thisBus->m_requests.emplace_back(r);
    m_thisBus->m_needTick = true;
}

void RequestBus::Send(const IRequestPtr& r)
{
    assert(m_thisBus);
    if (!r) return;
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ r->TypeInfo() });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    m_thisBus->InvokeHandlers(r, subscribers->second);
}

void RequestBus::InvokeHandlers(const IRequestPtr& r, const SubscriberList& subscribers)
{
    if (subscribers.empty()) return;
    for (auto subscriber : subscribers)
    {
        if (subscriber) subscriber->HandleRequest(r);
    }
}
