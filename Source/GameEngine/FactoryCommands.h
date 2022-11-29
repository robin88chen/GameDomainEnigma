/*********************************************************************
 * \file   FactoryCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _FACTORY_COMMANDS_H
#define _FACTORY_COMMANDS_H

#include "Frameworks/Command.h"
#include "GenericDtoFactories.h"
#include "GenericDto.h"

namespace Enigma::Engine
{
    class RegisterDtoFactory : public Frameworks::ICommand
    {
    public:
        RegisterDtoFactory(const std::string& rtti, const DtoFactory& factory)
            : m_rtti(rtti), m_factory(factory) {}

        const std::string& GetRtti() const { return m_rtti; }
        const DtoFactory& GetFactory() { return m_factory; }

    private:
        std::string m_rtti;
        DtoFactory m_factory;
    };
    class UnRegisterDtoFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterDtoFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& GetRtti() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
    class InvokeDtoFactory : public Frameworks::ICommand
    {
    public:
        InvokeDtoFactory(const GenericDto& dto) : m_dto(dto) {}

        const GenericDto& GetDto() { return m_dto; }

    private:
        GenericDto m_dto;
    };
}


#endif // _FACTORY_COMMANDS_H
