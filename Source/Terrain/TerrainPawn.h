/*********************************************************************
 * \file   TerrainPawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PAWN_H
#define TERRAIN_PAWN_H

#include "SceneGraph/Pawn.h"

namespace Enigma::Terrain
{
    class TerrainPawn : public SceneGraph::Pawn
    {
        DECLARE_EN_RTTI;
    public:
        TerrainPawn(const SceneGraph::SpatialId& id);
        TerrainPawn(const TerrainPawn&) = delete;
        TerrainPawn(TerrainPawn&&) = delete;
        TerrainPawn& operator=(const TerrainPawn&) = delete;
        TerrainPawn& operator=(TerrainPawn&&) = delete;
        virtual ~TerrainPawn() override;

        static std::shared_ptr<TerrainPawn> create(const SceneGraph::SpatialId& id);

        virtual std::shared_ptr<SceneGraph::SpatialAssembler> assembler() const override;
        virtual std::shared_ptr<SceneGraph::SpatialDisassembler> disassembler() const override;
    };
}

#endif // TERRAIN_PAWN_H
