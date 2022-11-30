/*********************************************************************
 * \file   EffectCompilingProfileDeserializer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _EFFECT_COMPILING_PROFILE_DESERIALIZER_H
#define _EFFECT_COMPILING_PROFILE_DESERIALIZER_H

#include "Frameworks/ruid.h"
#include <memory>
#include <string>

namespace Enigma::Engine
{
    class IEffectCompilingProfileDeserializer : public std::enable_shared_from_this<IEffectCompilingProfileDeserializer>
    {
    public:
        IEffectCompilingProfileDeserializer() = default;
        virtual ~IEffectCompilingProfileDeserializer() = default;

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) = 0;
    };
}

#endif // _EFFECT_COMPILING_PROFILE_DESERIALIZER_H
