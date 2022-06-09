/*********************************************************************
 * \file   CommandBus.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef COMMAND_BUS_H
#define COMMAND_BUS_H

#include "SystemService.h"
#include "Command.h"
#include "CommandSubscriber.h"
#include <list>
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace Enigma::Frameworks
{
    class IEvent;
    /** message service */
    class CommandBus : public ISystemService
    {
        DECLARE_RTTI;
    public:
        using CommandList = std::list<ICommandPtr>;
        using SubscriberList = std::list<CommandSubscriberPtr>;
        using CommandSubscriberMap = std::unordered_map<std::type_index, SubscriberList>;
    public:
        CommandBus(ServiceManager* manager);
        CommandBus(const CommandBus&) = delete;
        virtual ~CommandBus() override;
        CommandBus& operator=(const CommandBus&) = delete;

        virtual ServiceResult OnTick() override;
        virtual ServiceResult OnTerm() override;

        /** lambda 函式無法做互相比對 (operator==), 所以在 list 中無法做 remove,
        必須把每個 handler booking 一個 index, 據此來做 unregister,
        如果 handler 是實體 functor, 而不是 lambda, 就可以用來移除,
        所以, 用 subscriber 模式來實作
        */
        static void Subscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub);
        static void Unsubscribe(const std::type_info& cmd_type, const CommandSubscriberPtr& sub);

        static void Post(const ICommandPtr& c);
        static void Send(const ICommandPtr& c);

        void CleanupAllEvents();

    protected:
        void InvokeHandlers(const ICommandPtr& e, const SubscriberList& subscribers);

    protected:
        static CommandBus* m_thisBus;

        CommandSubscriberMap m_subscribers;
        CommandList m_commands;

        std::mutex m_commandListLock; ///< 需要執行緒鎖來鎖住 list的存取
    };
}


#endif // COMMAND_BUS_H
