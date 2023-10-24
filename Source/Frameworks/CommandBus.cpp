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

ServiceResult CommandBus::onTick()
{
    assert(m_thisBus);

    m_commandListLock.lock();
    unsigned int cmd_count = static_cast<unsigned int>(m_commands.size());
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

ServiceResult CommandBus::onTerm()
{
    CleanupAllCommands();

    return ServiceResult::Complete;
}

void CommandBus::Subscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub)
{
    assert(m_thisBus);
    auto type_index = std::type_index{ cmd_type };
    assert(m_thisBus->m_subscribers.find(type_index) == m_thisBus->m_subscribers.end());
    auto res = m_thisBus->m_subscribers.emplace(type_index, sub);
    assert(res.second);
}

void CommandBus::Unsubscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub)
{
    assert(m_thisBus);
    auto type_index = std::type_index{ cmd_type };
    assert(m_thisBus->m_subscribers.find(type_index) != m_thisBus->m_subscribers.end());
    auto res = m_thisBus->m_subscribers.erase(type_index);
    assert(res == 1);
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
    m_thisBus->InvokeHandler(c, subscribers->second);
}

void CommandBus::CleanupAllCommands()
{
    std::lock_guard<std::mutex> locker{ m_commandListLock };
    m_commands.clear();
}

void CommandBus::InvokeHandler(const ICommandPtr& c, const CommandSubscriberPtr& subscriber)
{
    if (subscriber) subscriber->HandleCommand(c);
}
