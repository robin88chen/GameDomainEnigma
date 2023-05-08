/*********************************************************************
 * \file   RequestBus.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _REQUEST_BUS_H
#define _REQUEST_BUS_H

#include "SystemService.h"
#include "Request.h"
#include "RequestSubscriber.h"
#include <list>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace Enigma::Frameworks
{
    /** message service */
    class RequestBus : public ISystemService
    {
        DECLARE_RTTI;
    public:
        using RequestList = std::list<IRequestPtr>;
        using SubscriberList = std::list<RequestSubscriberPtr>;
        using RequestSubscriberMap = std::unordered_map<std::type_index, SubscriberList>;
    public:
        RequestBus(ServiceManager* manager);
        RequestBus(const RequestBus&) = delete;
        RequestBus(RequestBus&&) = delete;
        virtual ~RequestBus() override;
        RequestBus& operator=(const RequestBus&) = delete;
        RequestBus& operator=(RequestBus&&) = delete;

        virtual ServiceResult OnTick() override;
        virtual ServiceResult OnTerm() override;

        /** lambda 函式無法做互相比對 (operator==), 所以在 list 中無法做 remove,
        必須把每個 handler booking 一個 index, 據此來做 unregister,
        如果 handler 是實體 functor, 而不是 lambda, 就可以用來移除,
        所以, 用 subscriber 模式來實作
        */
        static void Subscribe(const std::type_info& req_type, const RequestSubscriberPtr& sub);
        static void Unsubscribe(const std::type_info& req_type, const RequestSubscriberPtr& sub);

        static void Post(const IRequestPtr& r);
        static void Send(const IRequestPtr& r);

        void CleanupAllRequests();

    protected:
        void InvokeHandlers(const IRequestPtr& r, const SubscriberList& subscribers);

    protected:
        static RequestBus* m_thisBus;

        RequestSubscriberMap m_subscribers;
        RequestList m_requests;

        std::mutex m_requestListLock; ///< 需要執行緒鎖來鎖住 list的存取
    };
}

#endif // _REQUEST_BUS_H
