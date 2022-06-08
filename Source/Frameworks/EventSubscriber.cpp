#include "EventSubscriber.h"

Enigma::Frameworks::EventSubscriber::EventSubscriber()
{
}

Enigma::Frameworks::EventSubscriber::EventSubscriber(const EventHandler& handler) : m_handler(handler)
{
}

Enigma::Frameworks::EventSubscriber::~EventSubscriber()
{
    m_handler = nullptr;
}

void Enigma::Frameworks::EventSubscriber::HandleEvent(const IEventPtr& e)
{
    if (m_handler) m_handler(e);
}
