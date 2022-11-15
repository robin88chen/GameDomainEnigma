#include "ResponseBus.h"
#include <cassert>

using namespace Enigma::Frameworks;

DEFINE_RTTI(Frameworks, ResponseBus, ISystemService);

ResponseBus* ResponseBus::m_thisBus = nullptr;

ResponseBus::ResponseBus(ServiceManager* manager) : ISystemService(manager)
{
    assert(m_thisBus == nullptr);
    m_needTick = false;
    m_orderValue = MessageServiceOrderValue;
    m_thisBus = this;
}

ResponseBus::~ResponseBus()
{
    m_thisBus = nullptr;
}

ServiceResult ResponseBus::OnTick()
{
    assert(m_thisBus);

    unsigned int resp_count = 0;
    {
        std::lock_guard locker{ m_responseListLock };
        resp_count = static_cast<unsigned int>(m_responses.size());
    }
    if (resp_count == 0)
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    unsigned int resp_sended = 0;
    while (resp_sended < resp_count)
    {
        IResponsePtr resp = nullptr;
        {
            std::lock_guard locker{ m_responseListLock };
            if (!m_responses.empty())
            {
                resp = m_responses.front();
                m_responses.pop_front();
            }
        }
        if (!resp) break;
        Send(resp);
        resp_sended++;
    }
    return ServiceResult::Pendding;
}

ServiceResult ResponseBus::OnTerm()
{
    CleanupAllResponses();

    return ServiceResult::Complete;
}

void ResponseBus::Subscribe(const std::type_info& resp_type, const ResponseSubscriberPtr& sub)
{
    assert(m_thisBus);
    m_thisBus->m_subscribers[std::type_index{ resp_type }].emplace_back(sub);
}

void ResponseBus::Unsubscribe(const std::type_info& resp_type, const ResponseSubscriberPtr& sub)
{
    assert(m_thisBus);
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ resp_type });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    subscribers->second.remove(sub);
}

void ResponseBus::CleanupAllResponses()
{
    std::lock_guard locker{ m_responseListLock };
    m_responses.clear();
}

void ResponseBus::Post(const IResponsePtr& r)
{
    assert(m_thisBus);
    if (!r) return;
    if (m_thisBus->m_isSuspended) return;

    std::lock_guard locker{ m_thisBus->m_responseListLock };
    m_thisBus->m_responses.emplace_back(r);
    m_thisBus->m_needTick = true;
}

void ResponseBus::Send(const IResponsePtr& r)
{
    assert(m_thisBus);
    if (!r) return;
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ r->TypeInfo() });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    m_thisBus->InvokeHandlers(r, subscribers->second);
}

void ResponseBus::InvokeHandlers(const IResponsePtr& r, const SubscriberList& subscribers)
{
    if (subscribers.empty()) return;
    for (auto subscriber : subscribers)
    {
        if (subscriber) subscriber->HandleResponse(r);
    }
}
