/*********************************************************************
 * \file   AnimatedPawnDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATED_PAWN_DTO_H
#define _ANIMATED_PAWN_DTO_H

#include "SceneGraph/SceneGraphDtos.h"

namespace Enigma::GameCommon
{
    /*class AnimatedPawnDto : public SceneGraph::PawnDto
    {
    public:
        AnimatedPawnDto();
        AnimatedPawnDto(const Engine::GenericDto& dto);
        AnimatedPawnDto(const SceneGraph::PawnDto& dto);

        std::optional<Engine::GenericDto>& animationClipMapDto() { return m_animationClipMapDto; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& animationClipMapDto() const { return m_animationClipMapDto; }
        Engine::GenericDtoCollection& avatarRecipeDtos() { return m_avatarRecipeDtos; }
        [[nodiscard]] const Engine::GenericDtoCollection& avatarRecipeDtos() const { return m_avatarRecipeDtos; }

        Engine::GenericDto toGenericDto() const;

    private:
        std::optional<Engine::GenericDto> m_animationClipMapDto;
        Engine::GenericDtoCollection m_avatarRecipeDtos;
    };*/
}

#endif // _ANIMATED_PAWN_DTO_H
