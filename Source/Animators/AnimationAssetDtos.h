/*********************************************************************
 * \file   AnimationAssetDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_DTOS_H
#define _ANIMATION_ASSET_DTOS_H

#include "GameEngine/GenericDto.h"
#include <string>
#include <vector>

namespace Enigma::Animators
{
    class AnimationAssetPolicy;

    class AnimationTimeSRTDto
    {
    public:
        AnimationTimeSRTDto() = default;

        [[nodiscard]] const std::vector<float>& ScaleTimeKeys() const { return m_scaleTimeKeys; }
        std::vector<float>& ScaleTimeKeys() { return m_scaleTimeKeys; }
        [[nodiscard]] const std::vector<float>& RotateTimeKeys() const { return m_rotateTimeKeys; }
        std::vector<float>& RotateTimeKeys() { return m_rotateTimeKeys; }
        [[nodiscard]] const std::vector<float>& TranslateTimeKeys() const { return m_translateTimeKeys; }
        std::vector<float>& TranslateTimeKeys() { return m_translateTimeKeys; }

        static AnimationTimeSRTDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

    protected:
        std::vector<float> m_scaleTimeKeys;
        std::vector<float> m_rotateTimeKeys;
        std::vector<float> m_translateTimeKeys;
    };

    class AnimationAssetDto
    {
    public:
        AnimationAssetDto();

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factoryDesc;
    };

    class ModelAnimationAssetDto : public AnimationAssetDto
    {
    public:
        ModelAnimationAssetDto();

        [[nodiscard]] const std::vector<std::string>& MeshNodeNames() const { return m_meshNodeNames; }
        std::vector<std::string>& MeshNodeNames() { return m_meshNodeNames; }
        [[nodiscard]] const Engine::GenericDtoCollection& TimeSRTs() const { return m_timeSrtDtos; }
        Engine::GenericDtoCollection& TimeSRTs() { return m_timeSrtDtos; }

        static ModelAnimationAssetDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

    protected:
        std::vector<std::string> m_meshNodeNames;
        Engine::GenericDtoCollection m_timeSrtDtos;
    };
}

#endif // _ANIMATION_ASSET_DTOS_H
