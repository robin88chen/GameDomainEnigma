#include "QuerySubscriber.h"

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
