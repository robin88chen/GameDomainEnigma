/*********************************************************************
 * \file   ContractEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef CONTRACT_EVENTS_H
#define CONTRACT_EVENTS_H

#include "Frameworks/Event.h"
#include "Frameworks/ruid.h"
#include "Contract.h"

namespace Enigma::Engine
{
    class ContractDeserialized : public Frameworks::IEvent
    {
    public:
        ContractDeserialized(const Frameworks::Ruid& ruid, const Contract& contract) :
            m_ruid(ruid), m_contract(contract) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        const Contract& GetContract() { return m_contract; }
    private:
        Frameworks::Ruid m_ruid;
        Contract m_contract;
    };
}

#endif // CONTRACT_EVENTS_H
