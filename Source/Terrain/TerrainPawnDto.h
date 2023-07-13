/*********************************************************************
 * \file   TerrainPawnDto.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PAWN_DTO_H
#define TERRAIN_PAWN_DTO_H

#include "SceneGraph/SceneGraphDtos.h"
namespace Enigma::Terrain
{
    class TerrainPawnDto : public SceneGraph::PawnDto
    {
    public:
        TerrainPawnDto() = default;
        TerrainPawnDto(const SceneGraph::PawnDto& dto);

        static TerrainPawnDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;
    };
}

#endif // TERRAIN_PAWN_DTO_H
