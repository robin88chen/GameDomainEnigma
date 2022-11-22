#include "CommandBus.h"
#include <cassert>

using namespace Enigma::Frameworks;

DEFINE_RTTI(Frameworks, CommandBus, ISystemService);

CommandBus* CommandBus::m_thisBus = nullptr;

CommandBus::CommandBus(ServiceManager* manager) : ISystemService(manager)
{
    assert(m_thisBus == nullptr);
    m_needTick = false;
    m_orderValue = MessageServiceOrderValue;
    m_thisBus = this;
}

CommandBus::~CommandBus()
{
    m_thisBus = nullptr;
}

ServiceResult CommandBus::OnTick()
{
    assert(m_thisBus);

    unsigned int cmd_count = 0;
    m_commandListLock.lock();
    cmd_count = (unsigned int)m_commands.size();
    m_commandListLock.unlock();

    if (cmd_count == 0)
    {
        m_needTick = false;
        return ServiceResult::Pendding;
    }
    unsigned int cmd_sended = 0;
    while (cmd_sended < cmd_count)
    {
        ICommandPtr cmd = nullptr;
        m_commandListLock.lock();
        if (!m_commands.empty())
        {
            cmd = m_commands.front();
            m_commands.pop_front();
        }
        m_commandListLock.unlock();
        if (!cmd) break;
        Send(cmd);
        cmd_sended++;
    }
    return ServiceResult::Pendding;
}

ServiceResult CommandBus::OnTerm()
{
    CleanupAllCommands();

    return ServiceResult::Complete;
}

void CommandBus::Subscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub)
{
    assert(m_thisBus);
    m_thisBus->m_subscribers[std::type_index{ cmd_type }].emplace_back(sub);
}

void CommandBus::Unsubscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub)
{
    assert(m_thisBus);
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ cmd_type });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    subscribers->second.remove(sub);
}

void CommandBus::Post(const ICommandPtr& c)
{
    assert(m_thisBus);
    if (!c) return;
    if (m_thisBus->m_isSuspended) return;

    m_thisBus->m_commandListLock.lock();
    m_thisBus->m_commands.emplace_back(c);
    m_thisBus->m_commandListLock.unlock();
    m_thisBus->m_needTick = true;
}

void CommandBus::Send(const ICommandPtr& c)
{
    assert(m_thisBus);
    if (!c) return;
    auto subscribers = m_thisBus->m_subscribers.find(std::type_index{ c->TypeInfo() });
    if (subscribers == m_thisBus->m_subscribers.end()) return;
    m_thisBus->InvokeHandlers(c, subscribers->second);
}

void CommandBus::CleanupAllCommands()
{
    std::lock_guard<std::mutex> locker{ m_commandListLock };
    m_commands.clear();
}

void CommandBus::InvokeHandlers(const ICommandPtr& c, const SubscriberList& subscribers)
{
    if (subscribers.empty()) return;
    for (auto subscriber : subscribers)
    {
        if (subscriber) subscriber->HandleCommand(c);
    }
}
