#include "EventPublisher.h"
#include <cassert>

using namespace Enigma::Frameworks;

DEFINE_RTTI(Frameworks, EventPublisher, ISystemService);

EventPublisher* EventPublisher::m_thisPublisher = nullptr;

EventPublisher::EventPublisher(ServiceManager* manager) : ISystemService(manager)
{
    assert(m_thisPublisher == nullptr);
    m_needTick = false;
    m_thisPublisher = this;
}

EventPublisher::~EventPublisher()
{
    m_thisPublisher = nullptr;
}

ServiceResult EventPublisher::onTick()
{
    assert(m_thisPublisher);

    m_eventListLock.lock();
    unsigned int ev_count = static_cast<unsigned int>(m_events.size());
    m_eventListLock.unlock();

    if (ev_count == 0)
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    unsigned int ev_sended = 0;
    while (ev_sended < ev_count)
    {
        IEventPtr ev = nullptr;
        m_eventListLock.lock();
        if (!m_events.empty())
        {
            ev = m_events.front();
            m_events.pop_front();
        }
        m_eventListLock.unlock();
        if (!ev) break;
        publish(ev);
        ev_sended++;
    }
    return ServiceResult::Pendding;
}

ServiceResult EventPublisher::onTerm()
{
    cleanupAllEvents();

    return ServiceResult::Complete;
}

void EventPublisher::subscribe(const std::type_info& ev_type, const EventSubscriberPtr& sub)
{
    assert(m_thisPublisher);
    m_thisPublisher->m_subscribers[std::type_index{ ev_type }].emplace_back(sub);
}

void EventPublisher::unsubscribe(const std::type_info& ev_type, const EventSubscriberPtr& sub)
{
    assert(m_thisPublisher);
    auto subscribers = m_thisPublisher->m_subscribers.find(std::type_index{ ev_type });
    if (subscribers == m_thisPublisher->m_subscribers.end()) return;
    subscribers->second.remove(sub);
}

void EventPublisher::enqueue(const IEventPtr& e)
{
    assert(m_thisPublisher);
    if (!e) return;
    if (m_thisPublisher->m_isSuspended) return;

    m_thisPublisher->m_eventListLock.lock();
    m_thisPublisher->m_events.emplace_back(e);
    m_thisPublisher->m_eventListLock.unlock();
    m_thisPublisher->m_needTick = true;
}

void EventPublisher::publish(const IEventPtr& e)
{
    assert(m_thisPublisher);
    if (!e) return;
    auto subscribers = m_thisPublisher->m_subscribers.find(std::type_index{ e->typeInfo() });
    if (subscribers == m_thisPublisher->m_subscribers.end()) return;
    m_thisPublisher->invokeHandlers(e, subscribers->second);
}

void EventPublisher::cleanupAllEvents()
{
    std::lock_guard<std::mutex> locker{ m_eventListLock };
    m_events.clear();
}

void EventPublisher::invokeHandlers(const IEventPtr& e, const SubscriberList& subscribers)
{
    if (subscribers.empty()) return;
    for (auto subscriber : subscribers)
    {
        if (subscriber) subscriber->handleEvent(e);
    }
}
