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
#include "AnimationClipMapDto.h"

namespace Enigma::GameCommon
{
    class AnimatedPawnDto : public SceneGraph::PawnDto
    {
    public:
        AnimatedPawnDto() = default;
        AnimatedPawnDto(const SceneGraph::PawnDto& dto);
        virtual ~AnimatedPawnDto() = default;

        AnimationClipMapDto& TheAnimationClipMapDto() { return m_animationClipMapDto; }
        [[nodiscard]] const AnimationClipMapDto& TheAnimationClipMapDto() const { return m_animationClipMapDto; }
        std::vector<Engine::GenericDto>& AvatarRecipeDtos() { return m_avatarRecipeDtos; }
        [[nodiscard]] const std::vector<Engine::GenericDto>& AvatarRecipeDtos() const { return m_avatarRecipeDtos; }

        static AnimatedPawnDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    private:
        AnimationClipMapDto m_animationClipMapDto;
        std::vector<Engine::GenericDto> m_avatarRecipeDtos;
    };
}

#endif // _ANIMATED_PAWN_DTO_H
