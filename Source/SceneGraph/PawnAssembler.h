/*********************************************************************
 * \file   PawnAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef PAWN_ASSEMBLER_H
#define PAWN_ASSEMBLER_H

#include "Primitives/PrimitiveId.h"
#include "SpatialAssembler.h"

namespace Enigma::SceneGraph
{
    class PawnAssembler : public SpatialAssembler
    {
    public:
        PawnAssembler(const SpatialId& id);

        void primitiveId(const Primitives::PrimitiveId& primitive_id);

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<Primitives::PrimitiveId> m_primitiveId;
    };

    class PawnDisassembler : public SpatialDisassembler
    {
    public:
        PawnDisassembler();

        [[nodiscard]] std::optional<Primitives::PrimitiveId> primitiveId() const { return m_primitiveId; }
        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<Primitives::PrimitiveId> m_primitiveId;
    };
}

#endif // PAWN_ASSEMBLER_H
