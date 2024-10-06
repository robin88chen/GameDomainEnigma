#include "QuerySubscriber.h"
#include "QueryDispatcher.h"

using namespace Enigma::Frameworks;

QuerySubscriber::QuerySubscriber()
{
}

QuerySubscriber::QuerySubscriber(const QueryHandler& handler)
{
    m_handler = handler;
}

QuerySubscriber::~QuerySubscriber()
{
    m_handler = nullptr;
}

void QuerySubscriber::handleQuery(const IQueryPtr& q)
{
    if (m_handler) m_handler(q);
}

QuerySubscriberPtr Enigma::Frameworks::requestSubscription(const std::type_info& ev_type, const QueryHandler& handler)
{
    auto subscriber = std::make_shared<QuerySubscriber>(handler);
    QueryDispatcher::subscribe(ev_type, subscriber);
    return subscriber;
}

void Enigma::Frameworks::releaseSubscription(const std::type_info& ev_type, const QuerySubscriberPtr& subscriber)
{
    QueryDispatcher::unsubscribe(ev_type, subscriber);
}
