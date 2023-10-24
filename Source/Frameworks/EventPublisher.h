/*********************************************************************
 * \file   EventPublisher.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef EVENT_PUBLISHER_H
#define EVENT_PUBLISHER_H

#include "SystemService.h"
#include "Event.h"
#include "EventSubscriber.h"
#include <list>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace Enigma::Frameworks
{
    class IEvent;
    /** message service */
    class EventPublisher : public ISystemService
    {
        DECLARE_RTTI;
    public:
        using EventList = std::list<IEventPtr>;
        using SubscriberList = std::list<EventSubscriberPtr>;
        using EventSubscriberMap = std::unordered_map<std::type_index, SubscriberList>;
    public:
        EventPublisher(ServiceManager* manager);
        EventPublisher(const EventPublisher&) = delete;
        EventPublisher(EventPublisher&&) = delete;
        virtual ~EventPublisher() override;
        EventPublisher& operator=(const EventPublisher&) = delete;
        EventPublisher& operator=(EventPublisher&&) = delete;

        virtual ServiceResult onTick() override;
        virtual ServiceResult onTerm() override;

        /** lambda 函式無法做互相比對 (operator==), 所以在 list 中無法做 remove,
        必須把每個 handler booking 一個 index, 據此來做 unregister,
        如果 handler 是實體 functor, 而不是 lambda, 就可以用來移除,
        所以, 用 subscriber 模式來實作
        */
        static void Subscribe(const std::type_info& ev_type, const EventSubscriberPtr& sub);
        static void Unsubscribe(const std::type_info& ev_type, const EventSubscriberPtr& sub);

        static void Post(const IEventPtr& e);
        static void Send(const IEventPtr& e);

        void CleanupAllEvents();

    protected:
        void InvokeHandlers(const IEventPtr& e, const SubscriberList& subscribers);

    protected:
        static EventPublisher* m_thisPublisher;

        EventSubscriberMap m_subscribers;
        EventList m_events;

        std::mutex m_eventListLock; ///< 需要執行緒鎖來鎖住event list的存取
    };
}


#endif // EVENT_PUBLISHER_H
