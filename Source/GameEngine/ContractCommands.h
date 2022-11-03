/*********************************************************************
 * \file   ContractCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _CONTRACT_COMMANDS_H
#define _CONTRACT_COMMANDS_H

#include "Frameworks/Command.h"
#include "ContractedFactories.h"
#include "Contract.h"

namespace Enigma::Engine
{
    class RegisterContractFactory : public Frameworks::ICommand
    {
    public:
        RegisterContractFactory(const std::string& rtti, const ContractFactory& factory)
            : m_rtti(rtti), m_factory(factory) {}

        const std::string& GetRtti() const { return m_rtti; }
        const ContractFactory& GetFactory() { return m_factory; }

    private:
        std::string m_rtti;
        ContractFactory m_factory;
    };
    class UnRegisterContractFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterContractFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& GetRtti() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
    class InvokeContractFactory : public Frameworks::ICommand
    {
    public:
        InvokeContractFactory(const Contract& contract) : m_contract(contract) {}

        const Contract& GetContract() { return m_contract; }

    private:
        Contract m_contract;
    };
}


#endif // _CONTRACT_COMMANDS_H
