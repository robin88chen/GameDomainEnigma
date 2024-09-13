/*********************************************************************
 * \file   TerrainPawnAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef TERRAIN_PAWN_ASSEMBLER_H
#define TERRAIN_PAWN_ASSEMBLER_H

#include "SceneGraph/PawnAssembler.h"

namespace Enigma::Terrain
{
    class TerrainPawnAssembler : public SceneGraph::PawnAssembler
    {
    public:
        TerrainPawnAssembler(const SceneGraph::SpatialId& id);
        virtual ~TerrainPawnAssembler() override = default;
    };
    class TerrainPawnDisassembler : public SceneGraph::PawnDisassembler
    {
    public:
        TerrainPawnDisassembler();
        virtual ~TerrainPawnDisassembler() override = default;
    };
}

#endif // TERRAIN_PAWN_ASSEMBLER_H
