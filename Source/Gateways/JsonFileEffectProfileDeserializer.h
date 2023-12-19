/*********************************************************************
 * \file   JsonFileEffectProfileDeserializer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H
#define _JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H

#include <string>
#include "GameEngine/EffectCompilingProfileDeserializer.h"
#include "EffectProfileJsonGateway.h"

namespace Enigma::Gateways
{
    class JsonFileEffectProfileDeserializer : public Engine::IEffectCompilingProfileDeserializer
    {
    public:
        JsonFileEffectProfileDeserializer();

        virtual void invokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;

    protected:
        EffectProfileJsonGateway m_gateway;
    };
}

#endif // _JSON_FILE_EFFECT_PROFILE_DESERIALIZER_H
