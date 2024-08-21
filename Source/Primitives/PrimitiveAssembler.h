/*********************************************************************
 * \file   PrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef PRIMITIVE_ASSEMBLER_H
#define PRIMITIVE_ASSEMBLER_H

#include "PrimitiveId.h"
#include "GameEngine/GenericDto.h"
#include "GameEngine/FactoryDesc.h"

namespace Enigma::Primitives
{
    class PrimitiveAssembler
    {
    public:
        PrimitiveAssembler(const PrimitiveId& id);
        virtual ~PrimitiveAssembler() = default;
        PrimitiveAssembler(const PrimitiveAssembler&) = default;
        PrimitiveAssembler(PrimitiveAssembler&&) = default;
        PrimitiveAssembler& operator=(const PrimitiveAssembler&) = default;
        PrimitiveAssembler& operator=(PrimitiveAssembler&&) = default;

        void factoryDesc(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

        virtual Engine::GenericDto assemble() const = 0;

    protected:
        PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };

    class PrimitiveDisassembler
    {
    public:
        PrimitiveDisassembler();
        virtual ~PrimitiveDisassembler() = default;
        PrimitiveDisassembler(const PrimitiveDisassembler&) = default;
        PrimitiveDisassembler(PrimitiveDisassembler&&) = default;
        PrimitiveDisassembler& operator=(const PrimitiveDisassembler&) = default;
        PrimitiveDisassembler& operator=(PrimitiveDisassembler&&) = default;

        [[nodiscard]] const PrimitiveId& id() const { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }

        virtual void disassemble(const Engine::GenericDto& dto) = 0;

    protected:
        PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // PRIMITIVE_ASSEMBLER_H
