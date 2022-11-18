/*********************************************************************
 * \file   JsonFileContractDeserializer.h
 * \brief  contract deserializer for json file
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _JSON_FILE_CONTRACT_DESERIALIZER_H
#define _JSON_FILE_CONTRACT_DESERIALIZER_H

#include <string>
#include "GameEngine/ContractDeserializer.h"

namespace Enigma::Gateways
{
    class JsonFileContractDeserializer : public Engine::IContractDeserializer
    {
    public:
        JsonFileContractDeserializer();
        ~JsonFileContractDeserializer() override;

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;
    };
}

#endif // _JSON_FILE_CONTRACT_DESERIALIZER_H
