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
}

#endif // PRIMITIVE_ASSEMBLER_H
