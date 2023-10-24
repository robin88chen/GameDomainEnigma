/*********************************************************************
 * \file   CommandSubscriber.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef COMMAND_SUBSCRIBER_H
#define COMMAND_SUBSCRIBER_H

#include "Command.h"

namespace Enigma::Frameworks
{
    class CommandSubscriber
    {
    public:
        CommandSubscriber();
        CommandSubscriber(const CommandHandler& handler);
        CommandSubscriber(const CommandSubscriber&) = delete;
        CommandSubscriber(CommandSubscriber&&) = delete;
        virtual ~CommandSubscriber();
        CommandSubscriber& operator=(const CommandSubscriber&) = delete;
        CommandSubscriber& operator=(CommandSubscriber&&) = delete;

        virtual void handleCommand(const ICommandPtr& e);

    protected:
        CommandHandler m_handler;
    };
    using CommandSubscriberPtr = std::shared_ptr<CommandSubscriber>;
}


#endif // COMMAND_SUBSCRIBER_H
