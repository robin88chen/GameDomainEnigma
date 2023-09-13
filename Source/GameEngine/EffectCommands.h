/*********************************************************************
 * \file   EffectCommands.h
 * \brief  effect commands
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _EFFECT_COMMANDS_H
#define _EFFECT_COMMANDS_H

#include "Frameworks/Command.h"
#include "EffectMaterialDto.h"

namespace Enigma::Engine
{
    class CompileEffectMaterial : public Frameworks::IRequestCommand
    {
    public:
        CompileEffectMaterial(const EffectMaterialDto& dto) : m_dto(dto) {}

        const EffectMaterialDto& GetDto() const { return m_dto; }

    private:
        EffectMaterialDto m_dto;
    };
}

#endif // _EFFECT_COMMANDS_H
