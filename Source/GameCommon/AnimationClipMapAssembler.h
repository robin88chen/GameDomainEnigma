/*****************************************************************
 * \file   AnimationClipMapAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef ANIMATION_CLIP_MAP_ASSEMBLER_H
#define ANIMATION_CLIP_MAP_ASSEMBLER_H

#include "AnimationClipMap.h"
#include <vector>

namespace Enigma::GameCommon
{
    class AnimationClipMapAssembler
    {
    public:
        AnimationClipMapAssembler();
        AnimationClipMapAssembler(const AnimationClipMap& clipMap);

        void clipMap(const AnimationClipMap& clipMap);
        void addClip(const AnimationClipMap::AnimClip& clip);

        Engine::GenericDto assemble() const;

    private:
        Engine::FactoryDesc m_factoryDesc;
        std::vector<AnimationClipMap::AnimClip> m_clips;
    };

    class AnimationClipMapDisassembler
    {
    public:
        AnimationClipMapDisassembler();
        AnimationClipMapDisassembler(const Engine::GenericDto& dto);

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const AnimationClipMap& clipMap() const { return m_clipMap; }

    protected:
        Engine::FactoryDesc m_factoryDesc;
        AnimationClipMap m_clipMap;
    };
}

#endif // ANIMATION_CLIP_MAP_ASSEMBLER_H
