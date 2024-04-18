/*********************************************************************
 * \file   AnimationClipMapDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATION_CLIP_MAP_DTO_H
#define _ANIMATION_CLIP_MAP_DTO_H

#include <string>
#include <vector>
#include "GameEngine/GenericDto.h"

namespace Enigma::GameCommon
{
    class AnimationClipMapDto
    {
    public:
        AnimationClipMapDto();
        AnimationClipMapDto(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        std::vector<std::string>& animationNames() { return m_animNames; }
        [[nodiscard]] const std::vector<std::string>& animationNames() const { return m_animNames; }
        std::vector<float>& startOffsets() { return m_startOffsets; }
        [[nodiscard]] const std::vector<float>& startOffsets() const { return m_startOffsets; }
        std::vector<float>& loopTimes() { return m_loopTimes; }
        [[nodiscard]] const std::vector<float>& loopTimes() const { return m_loopTimes; }
        std::vector<unsigned>& warpModes() { return m_warpModes; }
        [[nodiscard]] const std::vector<unsigned>& warpModes() const { return m_warpModes; }
        std::vector<unsigned>& divideIndices() { return m_divideIndices; }
        [[nodiscard]] const std::vector<unsigned>& divideIndices() const { return m_divideIndices; }

        Engine::GenericDto toGenericDto() const;

    private:
        Engine::FactoryDesc m_factoryDesc;
        std::vector<std::string> m_animNames;
        std::vector<float> m_startOffsets;
        std::vector<float> m_loopTimes;
        std::vector<unsigned> m_warpModes;
        std::vector<unsigned> m_divideIndices;
    };
}

#endif // _ANIMATION_CLIP_MAP_DTO_H
