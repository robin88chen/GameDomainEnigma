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
        virtual ~CommandSubscriber();

        virtual void HandleCommand(const ICommandPtr& e);

    protected:
        CommandHandler m_handler;
    };
    using CommandSubscriberPtr = std::shared_ptr<CommandSubscriber>;
}


#endif // COMMAND_SUBSCRIBER_H
