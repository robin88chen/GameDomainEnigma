/*********************************************************************
 * \file   SkinMeshPrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef SKIN_MESH_PRIMITIVE_ASSEMBLER_H
#define SKIN_MESH_PRIMITIVE_ASSEMBLER_H

#include "MeshPrimitiveAssembler.h"

namespace Enigma::Renderables
{
    class SkinMeshPrimitiveAssembler : public MeshPrimitiveAssembler
    {
    public:
        SkinMeshPrimitiveAssembler(const Primitives::PrimitiveId& id);
        virtual ~SkinMeshPrimitiveAssembler() override = default;
        SkinMeshPrimitiveAssembler(const SkinMeshPrimitiveAssembler&) = default;
        SkinMeshPrimitiveAssembler(SkinMeshPrimitiveAssembler&&) = default;
        SkinMeshPrimitiveAssembler& operator=(const SkinMeshPrimitiveAssembler&) = default;
        SkinMeshPrimitiveAssembler& operator=(SkinMeshPrimitiveAssembler&&) = default;
    };

    class SkinMeshPrimitiveDisassembler : public MeshPrimitiveDisassembler
    {
    public:
        SkinMeshPrimitiveDisassembler();
        virtual ~SkinMeshPrimitiveDisassembler() override = default;
        SkinMeshPrimitiveDisassembler(const SkinMeshPrimitiveDisassembler&) = default;
        SkinMeshPrimitiveDisassembler(SkinMeshPrimitiveDisassembler&&) = default;
        SkinMeshPrimitiveDisassembler& operator=(const SkinMeshPrimitiveDisassembler&) = default;
        SkinMeshPrimitiveDisassembler& operator=(SkinMeshPrimitiveDisassembler&&) = default;
    };
}

#endif // SKIN_MESH_PRIMITIVE_ASSEMBLER_H
