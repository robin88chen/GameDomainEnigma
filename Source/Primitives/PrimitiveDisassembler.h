/*********************************************************************
 * \file   PrimitiveDisassembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef PRIMITIVE_DISASSEMBLER_H
#define PRIMITIVE_DISASSEMBLER_H

#include "GameEngine/GenericDto.h"

namespace Enigma::Primitives
{
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

#endif // PRIMITIVE_DISASSEMBLER_H
