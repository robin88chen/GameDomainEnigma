/*****************************************************************
 * \file   EffectCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 ******************************************************************/
#ifndef EFFECT_COMMANDS_H
#define EFFECT_COMMANDS_H

#include "EffectMaterialId.h"
#include "Frameworks/Command.h"

namespace Enigma::Engine
{
    class ReleaseEffectMaterial : public Frameworks::ICommand
    {
    public:
        ReleaseEffectMaterial(const EffectMaterialId& id) : m_id(id) {}
        const EffectMaterialId& id() const { return m_id; }
    private:
        EffectMaterialId m_id;
    };
}

#endif // EFFECT_COMMANDS_H
