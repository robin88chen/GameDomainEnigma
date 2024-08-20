/*********************************************************************
 * \file   AnimationTimeSRTAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef ANIMATION_TIME_SRT_ASSEMBLER_H
#define ANIMATION_TIME_SRT_ASSEMBLER_H

#include "AnimationTimeSRT.h"
#include <vector>

namespace Enigma::Renderables
{
    class AnimationTimeSRTAssembler
    {
    public:
        AnimationTimeSRTAssembler() = default;

        void addScaleKey(const AnimationTimeSRT::ScaleKey& key);
        void scaleKeys(const std::vector<AnimationTimeSRT::ScaleKey>& keys) { m_scaleKeys = keys; }
        void addRotationKey(const AnimationTimeSRT::RotationKey& key);
        void rotationKeys(const std::vector<AnimationTimeSRT::RotationKey>& keys) { m_rotationKeys = keys; }
        void addTranslationKey(const AnimationTimeSRT::TranslateKey& key);
        void translationKeys(const std::vector<AnimationTimeSRT::TranslateKey>& keys) { m_translationKeys = keys; }

        Engine::GenericDto assembleWithSorted();

        static std::shared_ptr<AnimationTimeSRTAssembler> make(const AnimationTimeSRT& srt);

    protected:
        std::vector<AnimationTimeSRT::ScaleKey> m_scaleKeys;
        std::vector<AnimationTimeSRT::RotationKey> m_rotationKeys;
        std::vector<AnimationTimeSRT::TranslateKey> m_translationKeys;
    };

    class AnimationTimeSRTDisassembler
    {
    public:
        AnimationTimeSRTDisassembler() = default;
        AnimationTimeSRTDisassembler(const Engine::GenericDto& dto);

        [[nodiscard]] const std::vector<AnimationTimeSRT::ScaleKey>& scaleKeys() const { return m_scaleKeys; }
        [[nodiscard]] const std::vector<AnimationTimeSRT::RotationKey>& rotationKeys() const { return m_rotationKeys; }
        [[nodiscard]] const std::vector<AnimationTimeSRT::TranslateKey>& translationKeys() const { return m_translationKeys; }

        void disassemble(const Engine::GenericDto& dto);

        static AnimationTimeSRT srt(const Engine::GenericDto& dto);

    protected:
        std::vector<AnimationTimeSRT::ScaleKey> m_scaleKeys;
        std::vector<AnimationTimeSRT::RotationKey> m_rotationKeys;
        std::vector<AnimationTimeSRT::TranslateKey> m_translationKeys;
    };
}

#endif // ANIMATION_TIME_SRT_ASSEMBLER_H
