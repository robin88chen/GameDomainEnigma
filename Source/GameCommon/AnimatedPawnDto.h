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
    class AnimatedPawnDto : public SceneGraph::PawnDto
    {
    public:
        AnimatedPawnDto();
        AnimatedPawnDto(const SceneGraph::PawnDto& dto);

        std::optional<Engine::GenericDto>& TheAnimationClipMapDto() { return m_animationClipMapDto; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& TheAnimationClipMapDto() const { return m_animationClipMapDto; }
        Engine::GenericDtoCollection& AvatarRecipeDtos() { return m_avatarRecipeDtos; }
        [[nodiscard]] const Engine::GenericDtoCollection& AvatarRecipeDtos() const { return m_avatarRecipeDtos; }

        static AnimatedPawnDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    private:
        std::optional<Engine::GenericDto> m_animationClipMapDto;
        Engine::GenericDtoCollection m_avatarRecipeDtos;
    };
}

#endif // _ANIMATED_PAWN_DTO_H
