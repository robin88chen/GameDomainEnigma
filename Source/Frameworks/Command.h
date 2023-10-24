/*********************************************************************
 * \file   Command.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef COMMAND_H
#define COMMAND_H

#include "ruid.h"
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
        virtual const std::type_info& typeInfo() { return typeid(*this); };  ///< 實作層的 type info
    };
    // merge request and command, need ruid to identity
    class IRequestCommand : public ICommand
    {
    public:
        IRequestCommand() : m_ruid(Ruid::Generate()) {};
        IRequestCommand(const Ruid& ruid) : m_ruid(ruid) {};
        IRequestCommand(const IRequestCommand&) = delete;
        IRequestCommand(IRequestCommand&&) = delete;
        virtual ~IRequestCommand() {};
        IRequestCommand& operator=(const IRequestCommand&) = delete;
        IRequestCommand& operator=(IRequestCommand&&) = delete;

        const Ruid& getRuid() const { return m_ruid; }

    protected:
        Ruid m_ruid;
    };

    using ICommandPtr = std::shared_ptr<ICommand>;
    using CommandHandler = std::function<void(const ICommandPtr&)>;
    using CommandHandlerDelegate = void(*)(const ICommandPtr&);
}


#endif // COMMAND_H
