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

        [[nodiscard]] const std::string& HostLightName() const { return m_hostLightName; }
        std::string& HostLightName() { return m_hostLightName; }

        static LightingPawnDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    private:
        std::string m_hostLightName;
    };
}

#endif // LIGHTING_PAWN_DTO_H
