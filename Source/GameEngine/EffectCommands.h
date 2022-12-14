/*********************************************************************
 * \file   EffectCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_COMMANDS_H
#define _EFFECT_COMMANDS_H

#include "Frameworks/Command.h"
#include "EffectMaterialPolicy.h"

namespace Enigma::Engine
{
    class CompileEffectMaterial : public Frameworks::ICommand
    {
    public:
        CompileEffectMaterial(const EffectMaterialPolicy& policy) : m_policy(policy) {}

        const EffectMaterialPolicy& GetPolicy() const { return m_policy; }

    private:
        EffectMaterialPolicy m_policy;
    };
}


#endif // _EFFECT_COMMANDS_H
