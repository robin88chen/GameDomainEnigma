#include "QueryDispatcher.h"
#include <cassert>

using namespace Enigma::Frameworks;

DEFINE_RTTI(Frameworks, QueryDispatcher, ISystemService);

QueryDispatcher* QueryDispatcher::m_thisDispatcher = nullptr;

QueryDispatcher::QueryDispatcher(ServiceManager* manager) : ISystemService(manager)
{
    assert(m_thisDispatcher == nullptr);
    m_needTick = false;
    m_thisDispatcher = this;
}

QueryDispatcher::~QueryDispatcher()
{
    m_thisDispatcher = nullptr;
}

void QueryDispatcher::subscribe(const std::type_info& cmd_type, const QuerySubscriberPtr& sub)
{
    assert(m_thisDispatcher);
    auto type_index = std::type_index{ cmd_type };
    assert(m_thisDispatcher->m_subscribers.find(type_index) == m_thisDispatcher->m_subscribers.end());
    const auto res = m_thisDispatcher->m_subscribers.emplace(type_index, sub);
    assert(res.second);
}

void QueryDispatcher::unsubscribe(const std::type_info& cmd_type, const QuerySubscriberPtr& sub)
{
    assert(m_thisDispatcher);
    const auto type_index = std::type_index{ cmd_type };
    assert(m_thisDispatcher->m_subscribers.find(type_index) != m_thisDispatcher->m_subscribers.end());
    auto res = m_thisDispatcher->m_subscribers.erase(type_index);
    assert(res == 1);
}

void QueryDispatcher::dispatch(const IQueryPtr& q)
{
    assert(m_thisDispatcher);
    assert(q);
    const auto subscriber = m_thisDispatcher->m_subscribers.find(std::type_index{ q->typeInfo() });
    if (subscriber == m_thisDispatcher->m_subscribers.end()) return;
    subscriber->second->handleQuery(q);
}

