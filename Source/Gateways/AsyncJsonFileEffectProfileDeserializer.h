/*********************************************************************
 * \file   AsyncJsonFileEffectProfileDeserializer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _ASYNC_JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H
#define _ASYNC_JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H

#include <string>
#include "GameEngine/EffectCompilingProfileDeserializer.h"
#include "EffectProfileJsonGateway.h"

namespace Enigma::Gateways
{
    class AsyncJsonFileEffectProfileDeserializer : public Engine::IEffectCompilingProfileDeserializer
    {
    public:
        AsyncJsonFileEffectProfileDeserializer();
        ~AsyncJsonFileEffectProfileDeserializer() override;

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;

    protected:
        void DeserializeProcedure();

    protected:
        Frameworks::Ruid m_ruid;
        std::string m_parameter;
        std::future<void> m_deserializing;
        EffectProfileJsonGateway m_gateway;
    };
}

#endif // _ASYNC_JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H
