/*********************************************************************
 * \file   ModelAnimationDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_ANIMATION_DTOS_H
#define _MODEL_ANIMATION_DTOS_H

#include "GameEngine/GenericDto.h"
#include <string>
#include <vector>


namespace Enigma::Renderables
{
    class AnimationTimeSRTDto
    {
    public:
        AnimationTimeSRTDto() = default;
        AnimationTimeSRTDto(const Engine::GenericDto& dto);

        [[nodiscard]] const std::vector<float>& scaleTimeKeys() const { return m_scaleTimeKeys; }
        std::vector<float>& scaleTimeKeys() { return m_scaleTimeKeys; }
        [[nodiscard]] const std::vector<float>& rotateTimeKeys() const { return m_rotateTimeKeys; }
        std::vector<float>& rotateTimeKeys() { return m_rotateTimeKeys; }
        [[nodiscard]] const std::vector<float>& translateTimeKeys() const { return m_translateTimeKeys; }
        std::vector<float>& translateTimeKeys() { return m_translateTimeKeys; }

        Engine::GenericDto toGenericDto();

    protected:
        std::vector<float> m_scaleTimeKeys;
        std::vector<float> m_rotateTimeKeys;
        std::vector<float> m_translateTimeKeys;
    };

    class ModelAnimationAssetDto //: public Animators::AnimationAssetDto
    {
    public:
        ModelAnimationAssetDto();
        ModelAnimationAssetDto(const Engine::GenericDto& dto);

        [[nodiscard]] const std::vector<std::string>& meshNodeNames() const { return m_meshNodeNames; }
        std::vector<std::string>& meshNodeNames() { return m_meshNodeNames; }
        [[nodiscard]] const Engine::GenericDtoCollection& timeSRTs() const { return m_timeSrtDtos; }
        Engine::GenericDtoCollection& timeSRTs() { return m_timeSrtDtos; }

        Engine::GenericDto toGenericDto();

    protected:
        std::vector<std::string> m_meshNodeNames;
        Engine::GenericDtoCollection m_timeSrtDtos;
    };
}

#endif // _MODEL_ANIMATION_DTOS_H
