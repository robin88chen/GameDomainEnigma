/*********************************************************************
 * \file   ResponseBus.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _RESPONSE_BUS_H
#define _RESPONSE_BUS_H

#include "SystemService.h"
#include "Response.h"
#include "ResponseSubscriber.h"
#include <list>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace Enigma::Frameworks
{
    /** message service */
    class ResponseBus : public ISystemService
    {
        DECLARE_RTTI;
    public:
        using ResponseList = std::list<IResponsePtr>;
        using SubscriberList = std::list<ResponseSubscriberPtr>;
        using ResponseSubscriberMap = std::unordered_map<std::type_index, SubscriberList>;
    public:
        ResponseBus(ServiceManager* manager);
        ResponseBus(const ResponseBus&) = delete;
        virtual ~ResponseBus() override;
        ResponseBus& operator=(const ResponseBus&) = delete;

        virtual ServiceResult OnTick() override;
        virtual ServiceResult OnTerm() override;

        /** lambda 函式無法做互相比對 (operator==), 所以在 list 中無法做 remove,
        必須把每個 handler booking 一個 index, 據此來做 unregister,
        如果 handler 是實體 functor, 而不是 lambda, 就可以用來移除,
        所以, 用 subscriber 模式來實作
        */
        static void Subscribe(const std::type_info& resp_type, const ResponseSubscriberPtr& sub);
        static void Unsubscribe(const std::type_info& resp_type, const ResponseSubscriberPtr& sub);

        static void Post(const IResponsePtr& r);
        static void Send(const IResponsePtr& r);

        void CleanupAllResponses();

    protected:
        void InvokeHandlers(const IResponsePtr& r, const SubscriberList& subscribers);

    protected:
        static ResponseBus* m_thisBus;

        ResponseSubscriberMap m_subscribers;
        ResponseList m_responses;

        std::mutex m_responseListLock; ///< 需要執行緒鎖來鎖住 list的存取
    };
}

#endif // _RESPONSE_BUS_H
