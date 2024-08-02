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

namespace Enigma::Rendering
{
    class LightingPawnDto : public SceneGraph::PawnDto
    {
    public:
        LightingPawnDto(const Engine::GenericDto& dto);
        LightingPawnDto(const SceneGraph::PawnDto& dto);

        [[nodiscard]] const SceneGraph::SpatialId& hostLightId() const { return m_hostLightId; }
        void hostLightId(const  SceneGraph::SpatialId& id) { m_hostLightId = id; }

        Engine::GenericDto toGenericDto() const;

    private:
        SceneGraph::SpatialId m_hostLightId;
    };
}

#endif // LIGHTING_PAWN_DTO_H
