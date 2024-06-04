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
    class ICommand : public std::enable_shared_from_this<ICommand>
    {
    public:
        ICommand() {};
        ICommand(const ICommand&) = delete;
        ICommand(ICommand&&) = delete;
        virtual ~ICommand() {};
        ICommand& operator=(const ICommand&) = delete;
        ICommand& operator=(ICommand&&) = delete;
        virtual const std::type_info& typeInfo() { return typeid(*this); };  ///< 實作層的 type info
        //! ADR : 執行命令，預設是由註冊在 CommandBus 的 handler 來執行。concrete command 可以改寫為直接實作執行邏輯，簡單又單純，但就沒有置換 handler 的機會了。
        virtual void execute();
        void enqueue();
    };

    using ICommandPtr = std::shared_ptr<ICommand>;
    using CommandHandler = std::function<void(const ICommandPtr&)>;
    using CommandHandlerDelegate = void(*)(const ICommandPtr&);
}


#endif // COMMAND_H
