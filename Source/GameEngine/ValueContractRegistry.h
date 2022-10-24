/*********************************************************************
 * \file   ValueContractRegistry.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _ENGINE_VALUE_CONTRACT_REGISTRY_H
#define _ENGINE_VALUE_CONTRACT_REGISTRY_H

namespace Enigma::Engine
{
    class ValueContractRegistry
    {
    public:
        static void RegisterValueFactories();
        static void UnregisterValueFactories();
    };
}

#endif // _ENGINE_VALUE_CONTRACT_REGISTRY_H
