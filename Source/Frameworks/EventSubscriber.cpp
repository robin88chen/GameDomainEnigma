#include "EventSubscriber.h"

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

void EventSubscriber::HandleEvent(const IEventPtr& e)
{
    if (m_handler) m_handler(e);
}
