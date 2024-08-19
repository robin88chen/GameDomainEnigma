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

namespace Enigma::Primitives
{
    class PrimitiveAssembler
    {
    public:
        PrimitiveAssembler(const PrimitiveId& id) : m_id(id) {}
        virtual ~PrimitiveAssembler() = default;
        PrimitiveAssembler(const PrimitiveAssembler&) = default;
        PrimitiveAssembler(PrimitiveAssembler&&) = default;
        PrimitiveAssembler& operator=(const PrimitiveAssembler&) = default;
        PrimitiveAssembler& operator=(PrimitiveAssembler&&) = default;

        virtual Engine::GenericDto assemble() const = 0;
    protected:
        PrimitiveId m_id;
    };

    class PrimitiveDisassembler
    {
    public:
        PrimitiveDisassembler() = default;
        virtual ~PrimitiveDisassembler() = default;
        PrimitiveDisassembler(const PrimitiveDisassembler&) = default;
        PrimitiveDisassembler(PrimitiveDisassembler&&) = default;
        PrimitiveDisassembler& operator=(const PrimitiveDisassembler&) = default;
        PrimitiveDisassembler& operator=(PrimitiveDisassembler&&) = default;

        virtual void disassemble(const Engine::GenericDto& dto) = 0;
    };
}

#endif // PRIMITIVE_ASSEMBLER_H
