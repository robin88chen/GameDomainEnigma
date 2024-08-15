/*********************************************************************
 * \file   TriangleListAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef TRIANGLE_LIST_ASSEMBLER_H
#define TRIANGLE_LIST_ASSEMBLER_H

#include "GeometryAssembler.h"

namespace Enigma::Geometries
{
    class TriangleListAssembler : public GeometryAssembler
    {
    public:
        TriangleListAssembler(const GeometryId& id);
        virtual ~TriangleListAssembler() override = default;
        TriangleListAssembler(const TriangleListAssembler&) = default;
        TriangleListAssembler(TriangleListAssembler&&) = default;
        TriangleListAssembler& operator=(const TriangleListAssembler&) = default;
        TriangleListAssembler& operator=(TriangleListAssembler&&) = default;

        virtual void topology(Graphics::PrimitiveTopology topology) override;
    };

    class TriangleListDisassembler : public GeometryDisassembler
    {
    public:
        TriangleListDisassembler();
        virtual ~TriangleListDisassembler() override = default;
        TriangleListDisassembler(const TriangleListDisassembler&) = default;
        TriangleListDisassembler(TriangleListDisassembler&&) = default;
        TriangleListDisassembler& operator=(const TriangleListDisassembler&) = default;
        TriangleListDisassembler& operator=(TriangleListDisassembler&&) = default;

        virtual void disassemble(const Engine::GenericDto& dto) override;
    };
}

#endif // TRIANGLE_LIST_ASSEMBLER_H
