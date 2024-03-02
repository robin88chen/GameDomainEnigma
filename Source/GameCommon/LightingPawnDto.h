/*********************************************************************
 * \file   LightingPawnDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LIGHTING_PAWN_DTO_H
#define LIGHTING_PAWN_DTO_H

#include "SceneGraph/SceneGraphDtos.h"

namespace Enigma::GameCommon
{
    class LightingPawnDto : public SceneGraph::PawnDto
    {
    public:
        LightingPawnDto();
        LightingPawnDto(const SceneGraph::PawnDto& dto);

        [[nodiscard]] const SceneGraph::SpatialId& hostLightId() const { return m_hostLightId; }
        SceneGraph::SpatialId& hostLightId() { return m_hostLightId; }

        static LightingPawnDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    private:
        SceneGraph::SpatialId m_hostLightId;
    };
}

#endif // LIGHTING_PAWN_DTO_H
