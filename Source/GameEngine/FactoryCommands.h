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

        const std::string& getRtti() const { return m_rtti; }
        const DtoFactory& GetFactory() { return m_factory; }

    private:
        std::string m_rtti;
        DtoFactory m_factory;
    };
    class UnRegisterDtoFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterDtoFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& getRtti() const { return m_rtti; }

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
    class RegisterDtoPolicyConverter : public Frameworks::ICommand
    {
    public:
        RegisterDtoPolicyConverter(const std::string& rtti, const GenericPolicyConverter& converter)
            : m_rtti(rtti), m_converter(converter) {}

        const std::string& getRtti() const { return m_rtti; }
        const GenericPolicyConverter& GetPolicyConverter() { return m_converter; }

    private:
        std::string m_rtti;
        GenericPolicyConverter m_converter;
    };
    class UnRegisterDtoPolicyConverter : public Frameworks::ICommand
    {
    public:
        UnRegisterDtoPolicyConverter(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& getRtti() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}

#endif // _FACTORY_COMMANDS_H
