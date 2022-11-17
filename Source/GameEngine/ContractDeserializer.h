/*********************************************************************
 * \file   ContractDeserializer.h
 * \brief  contract deserializer, a base class, will implement by gateways
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _CONTRACT_DESERIALIZER_H
#define _CONTRACT_DESERIALIZER_H

#include "Frameworks/ruid.h"

namespace Enigma::Engine
{
    class IContractDeserializer
    {
    public:
        IContractDeserializer() = default;

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing) = 0;
    };
}

#endif // _CONTRACT_DESERIALIZER_H
