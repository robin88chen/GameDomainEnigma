/*********************************************************************
 * \file   EventSubscriber.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef EVENT_SUBSCRIBER_H
#define EVENT_SUBSCRIBER_H

#include "Event.h"

namespace Enigma::Frameworks
{
    class EventSubscriber
    {
    public:
        EventSubscriber();
        EventSubscriber(const EventHandler& handler);
        virtual ~EventSubscriber();

        virtual void HandleEvent(const IEventPtr& e);

    protected:
        EventHandler m_handler;
    };
    using EventSubscriberPtr = std::shared_ptr<EventSubscriber>;
}

#endif // EVENT_SUBSCRIBER_H
