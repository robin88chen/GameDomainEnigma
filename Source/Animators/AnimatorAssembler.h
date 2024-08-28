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
        AnimatorAssembler(const AnimatorId& id);
        virtual ~AnimatorAssembler() = default;
        AnimatorAssembler(const AnimatorAssembler&) = default;
        AnimatorAssembler(AnimatorAssembler&&) = default;
        AnimatorAssembler& operator=(const AnimatorAssembler&) = default;
        AnimatorAssembler& operator=(AnimatorAssembler&&) = default;

        virtual Engine::GenericDto assemble() const = 0;

        void factoryDesc(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

    protected:
        AnimatorId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
    class AnimatorDisassembler
    {
    public:
        AnimatorDisassembler();
        virtual ~AnimatorDisassembler() = default;
        AnimatorDisassembler(const AnimatorDisassembler&) = default;
        AnimatorDisassembler(AnimatorDisassembler&&) = default;
        AnimatorDisassembler& operator=(const AnimatorDisassembler&) = default;
        AnimatorDisassembler& operator=(AnimatorDisassembler&&) = default;

        [[nodiscard]] const AnimatorId& id() const { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }

        virtual void disassemble(const Engine::GenericDto& dto) = 0;

    protected:
        AnimatorId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // ANIMATOR_ASSEMBLER_H
