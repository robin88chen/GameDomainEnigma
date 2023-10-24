/*********************************************************************
 * \file   QuerySubscriber.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef QUERY_SUBSCRIBER_H
#define QUERY_SUBSCRIBER_H

#include "Query.h"

namespace Enigma::Frameworks
{
    class QuerySubscriber
    {
    public:
        QuerySubscriber();
        QuerySubscriber(const QueryHandler& handler);
        QuerySubscriber(const QuerySubscriber&) = delete;
        QuerySubscriber(QuerySubscriber&&) = delete;
        virtual ~QuerySubscriber();
        QuerySubscriber& operator=(const QuerySubscriber&) = delete;
        QuerySubscriber& operator=(QuerySubscriber&&) = delete;

        virtual void handleQuery(const IQueryPtr& e);

    protected:
        QueryHandler m_handler;
    };
    using QuerySubscriberPtr = std::shared_ptr<QuerySubscriber>;
}

#endif // QUERY_SUBSCRIBER_H
