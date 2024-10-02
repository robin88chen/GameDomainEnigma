#include "EventSubscriber.h"
#include "EventPublisher.h"

using namespace Enigma::Frameworks;

EventSubscriber::EventSubscriber()
{
}

EventSubscriber::EventSubscriber(const EventHandler& handler) : m_handler(handler)
{
}

EventSubscriber::~EventSubscriber()
{
    m_handler = nullptr;
}

void EventSubscriber::handleEvent(const IEventPtr& e)
{
    if (m_handler) m_handler(e);
}

EventSubscriberPtr Enigma::Frameworks::eventSubscription(const std::type_info& ev_type, const EventHandler& handler)
{
    auto subscriber = std::make_shared<EventSubscriber>(handler);
    EventPublisher::subscribe(ev_type, subscriber);
    return subscriber;
}

void Enigma::Frameworks::releaseSubscription(const std::type_info& ev_type, const EventSubscriberPtr& subscriber)
{
    EventPublisher::unsubscribe(ev_type, subscriber);
}
