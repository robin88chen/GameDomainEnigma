/*********************************************************************
 * \file   AsyncJsonFileContractDeserializer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _ASYNC_JSON_FILE_CONTRACT_DESERIALIZER_H
#define _ASYNC_JSON_FILE_CONTRACT_DESERIALIZER_H

#include <string>
#include <future>
#include "GameEngine/ContractDeserializer.h"

namespace Enigma::Gateways
{
    class AsyncJsonFileContractDeserializer : public Engine::IContractDeserializer
    {
    public:
        AsyncJsonFileContractDeserializer();
        ~AsyncJsonFileContractDeserializer() override;

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;
    protected:
        void DeserializeProcedure();

    protected:
        Frameworks::Ruid m_ruid;
        std::string m_parameter;
        std::future<void> m_deserializing;
    };
}

#endif // _ASYNC_JSON_FILE_CONTRACT_DESERIALIZER_H
