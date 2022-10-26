/*********************************************************************
 * \file   ContractedFactories.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CONTRACTED_FACTORIES_H
#define CONTRACTED_FACTORIES_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include <functional>
#include <unordered_map>

namespace Enigma::Engine
{
    class Contract;

    using ContractFactory = std::function<void(const Contract& contract)>;

    class ContractedFactories : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ContractedFactories(Frameworks::ServiceManager* srv_mngr);
        ContractedFactories(const ContractedFactories&) = delete;
        ContractedFactories(ContractedFactories&&) = delete;
        ~ContractedFactories();
        ContractedFactories& operator=(const ContractedFactories&) = delete;
        ContractedFactories& operator=(ContractedFactories&&) = delete;

        void RegisterFactory(const std::string& rtti, const ContractFactory& factory);
        void UnregisterFactory(const std::string& rtti);

        void InvokeFactory(const Contract& contract);

    private:
        void DoRegisteringFactory(const Frameworks::ICommandPtr& c);
        void DoUnRegisteringFactory(const Frameworks::ICommandPtr& c);
        void DoInvokingContractFactory(const Frameworks::ICommandPtr& c);

    private:
        using ContractFactoryTable = std::unordered_map<std::string, ContractFactory>;
        ContractFactoryTable m_factories;

        Frameworks::CommandSubscriberPtr m_doRegisteringFactory;
        Frameworks::CommandSubscriberPtr m_doUnRegisteringFactory;
        Frameworks::CommandSubscriberPtr m_doInvokingContractFactory;
    };
}

#endif // CONTRACTED_FACTORIES_H
