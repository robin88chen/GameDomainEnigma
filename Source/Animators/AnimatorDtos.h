/*********************************************************************
 * \file   AnimatorDtos.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef ANIMATOR_DTOS_H
#define ANIMATOR_DTOS_H

#include "GameEngine/GenericDto.h"

namespace Enigma::Animators
{
    class ModelAnimatorDto
    {
    public:
        ModelAnimatorDto() = default;

        [[nodiscard]] const Engine::GenericDto& AnimationAssetDto() const { return m_animationAssetDto; }
        Engine::GenericDto& AnimationAssetDto() { return m_animationAssetDto; }

    private:
        Engine::GenericDto m_animationAssetDto;
    };
}

#endif // ANIMATOR_DTOS_H
