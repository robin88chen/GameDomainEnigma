/*********************************************************************
 * \file   AnimatorAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef ANIMATOR_ASSEMBLER_H
#define ANIMATOR_ASSEMBLER_H

#include "AnimatorId.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Animators
{
    class AnimatorAssembler
    {
    public:
        AnimatorAssembler(const AnimatorId& id) : m_id(id) {}
        virtual ~AnimatorAssembler() = default;
        AnimatorAssembler(const AnimatorAssembler&) = default;
        AnimatorAssembler(AnimatorAssembler&&) = default;
        AnimatorAssembler& operator=(const AnimatorAssembler&) = default;
        AnimatorAssembler& operator=(AnimatorAssembler&&) = default;

        virtual Engine::GenericDto assemble() const = 0;

    protected:
        AnimatorId m_id;
    };
    class AnimatorDisassembler
    {
    public:
        AnimatorDisassembler() = default;
        virtual ~AnimatorDisassembler() = default;
        AnimatorDisassembler(const AnimatorDisassembler&) = default;
        AnimatorDisassembler(AnimatorDisassembler&&) = default;
        AnimatorDisassembler& operator=(const AnimatorDisassembler&) = default;
        AnimatorDisassembler& operator=(AnimatorDisassembler&&) = default;

        virtual void disassemble(const Engine::GenericDto& dto) = 0;
    };
}

#endif // ANIMATOR_ASSEMBLER_H
