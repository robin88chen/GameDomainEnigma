/*****************************************************************
 * \file   TerrainPrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 ******************************************************************/
#ifndef TERRAIN_PRIMITIVE_ASSEMBLER_H
#define TERRAIN_PRIMITIVE_ASSEMBLER_H

#include "Renderables/MeshPrimitiveAssembler.h"

namespace Enigma::Terrain
{
    class TerrainPrimitiveAssembler : public Renderables::MeshPrimitiveAssembler
    {
    public:
        TerrainPrimitiveAssembler(const Primitives::PrimitiveId& id);
        virtual ~TerrainPrimitiveAssembler() override = default;
        TerrainPrimitiveAssembler(const TerrainPrimitiveAssembler&) = default;
        TerrainPrimitiveAssembler(TerrainPrimitiveAssembler&&) = default;
        TerrainPrimitiveAssembler& operator=(const TerrainPrimitiveAssembler&) = default;
        TerrainPrimitiveAssembler& operator=(TerrainPrimitiveAssembler&&) = default;
        // assemble method is the same as the parent class
    };

    class TerrainPrimitiveDisassembler : public Renderables::MeshPrimitiveDisassembler
    {
    public:
        TerrainPrimitiveDisassembler();
        virtual ~TerrainPrimitiveDisassembler() override = default;
        TerrainPrimitiveDisassembler(const TerrainPrimitiveDisassembler&) = default;
        TerrainPrimitiveDisassembler(TerrainPrimitiveDisassembler&&) = default;
        TerrainPrimitiveDisassembler& operator=(const TerrainPrimitiveDisassembler&) = default;
        TerrainPrimitiveDisassembler& operator=(TerrainPrimitiveDisassembler&&) = default;
    };
}

#endif // TERRAIN_PRIMITIVE_ASSEMBLER_H
