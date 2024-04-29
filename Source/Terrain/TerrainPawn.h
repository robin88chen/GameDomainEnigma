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
        TerrainPawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        TerrainPawn(const TerrainPawn&) = delete;
        TerrainPawn(TerrainPawn&&) = delete;
        TerrainPawn& operator=(const TerrainPawn&) = delete;
        TerrainPawn& operator=(TerrainPawn&&) = delete;
        virtual ~TerrainPawn() override;

        static std::shared_ptr<TerrainPawn> create(const SceneGraph::SpatialId& id);
        static std::shared_ptr<TerrainPawn> constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);

        virtual Engine::GenericDto serializeDto() override;
    };
}

#endif // TERRAIN_PAWN_H
