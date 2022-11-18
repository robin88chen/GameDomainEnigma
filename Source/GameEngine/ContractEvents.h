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
        ContractDeserialized(const Frameworks::Ruid& ruid, const std::vector<Contract>& contracts) :
            m_ruid(ruid), m_contracts(contracts) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        const std::vector<Contract>& GetContracts() { return m_contracts; }
    private:
        Frameworks::Ruid m_ruid;
        std::vector<Contract> m_contracts;
    };
    class DeserializeContractFailed : public Frameworks::IEvent
    {
    public:
        DeserializeContractFailed(const Frameworks::Ruid& ruid, std::error_code er) :
            m_ruid(ruid), m_error(er) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::error_code m_error;
    };
}

#endif // CONTRACT_EVENTS_H
