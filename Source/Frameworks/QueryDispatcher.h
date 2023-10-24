/*********************************************************************
 * \file   QueryDispatcher.h
 * \brief  只做同步機制
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef QUERY_DISPATCHER_H
#define QUERY_DISPATCHER_H

#include "SystemService.h"
#include "Query.h"
#include "QuerySubscriber.h"
#include <unordered_map>
#include <typeindex>

namespace Enigma::Frameworks
{
    /** query dispatcher */
    class QueryDispatcher : public ISystemService
    {
        DECLARE_RTTI;
    public:
        /** one query must has only one subscriber */
        using QuerySubscriberMap = std::unordered_map<std::type_index, QuerySubscriberPtr>;
    public:
        QueryDispatcher(ServiceManager* manager);
        QueryDispatcher(const QueryDispatcher&) = delete;
        QueryDispatcher(QueryDispatcher&&) = delete;
        virtual ~QueryDispatcher() override;
        QueryDispatcher& operator=(const QueryDispatcher&) = delete;
        QueryDispatcher& operator=(QueryDispatcher&&) = delete;

        /** lambda 函式無法做互相比對 (operator==), 所以在 list 中無法做 remove,
        必須把每個 handler booking 一個 index, 據此來做 unregister,
        如果 handler 是實體 functor, 而不是 lambda, 就可以用來移除,
        所以, 用 subscriber 模式來實作
        */
        static void subscribe(const std::type_info& cmd_type, const QuerySubscriberPtr& sub);
        static void unsubscribe(const std::type_info& cmd_type, const QuerySubscriberPtr& sub);

        static void dispatch(const IQueryPtr& q);

    protected:
        static QueryDispatcher* m_thisDispatcher;

        QuerySubscriberMap m_subscribers;
    };
}

#endif // QUERY_DISPATCHER_H
