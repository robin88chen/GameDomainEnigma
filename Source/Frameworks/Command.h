/*********************************************************************
 * \file   Command.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <memory>

namespace Enigma::Frameworks
{
    class ICommand
    {
    public:
        ICommand() {};
        ICommand(const ICommand&) = delete;
        ICommand(ICommand&&) = delete;
        virtual ~ICommand() {};
        ICommand& operator=(const ICommand&) = delete;
        ICommand& operator=(ICommand&&) = delete;
        virtual const std::type_info& TypeInfo() { return typeid(*this); };  ///< 實作層的 type info
    };
    using ICommandPtr = std::shared_ptr<ICommand>;
    using CommandHandler = std::function<void(const ICommandPtr&)>;
    using CommandHandlerDelegate = void(*)(const ICommandPtr&);
}


#endif // COMMAND_H
