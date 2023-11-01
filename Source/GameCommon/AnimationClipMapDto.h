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

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        std::vector<std::string>& AnimNames() { return m_animNames; }
        [[nodiscard]] const std::vector<std::string>& AnimNames() const { return m_animNames; }
        std::vector<float>& StartOffsets() { return m_startOffsets; }
        [[nodiscard]] const std::vector<float>& StartOffsets() const { return m_startOffsets; }
        std::vector<float>& LoopTimes() { return m_loopTimes; }
        [[nodiscard]] const std::vector<float>& LoopTimes() const { return m_loopTimes; }
        std::vector<unsigned>& WarpModes() { return m_warpModes; }
        [[nodiscard]] const std::vector<unsigned>& WarpModes() const { return m_warpModes; }
        std::vector<unsigned>& DivideIndices() { return m_divideIndices; }
        [[nodiscard]] const std::vector<unsigned>& DivideIndices() const { return m_divideIndices; }

        static AnimationClipMapDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

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
