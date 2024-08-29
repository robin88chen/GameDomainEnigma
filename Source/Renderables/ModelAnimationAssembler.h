/*********************************************************************
 * \file   ModelAnimationAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef MODEL_ANIMATION_ASSEMBLER_H
#define MODEL_ANIMATION_ASSEMBLER_H

#include "Animators/AnimationAssetId.h"
#include "Animators/AnimationAssetAssembler.h"
#include "ModelAnimationAsset.h"
#include "AnimationTimeSRT.h"

namespace Enigma::Renderables
{
    class ModelAnimationAsset;
    class AnimationTimeSRTAssembler;
    class AnimationTimeSRTDisassembler;

    class ModelAnimationAssembler : public Animators::AnimationAssetAssembler
    {
    public:
        ModelAnimationAssembler(const Animators::AnimationAssetId& id);

        void nodeSRT(const std::string& node_name, const AnimationTimeSRT& srt);
        void nodeSRT(const std::string& node_name, const std::shared_ptr<AnimationTimeSRTAssembler>& assembler);
        void asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::vector<std::tuple<std::string, std::shared_ptr<AnimationTimeSRTAssembler>>> m_nodeSRTs;
    };

    class ModelAnimationDisassembler : public Animators::AnimationAssetDisassembler
    {
    public:
        ModelAnimationDisassembler();

        [[nodiscard]] const std::vector<std::tuple<std::string, AnimationTimeSRT>>& nodeSRTs() const { return m_nodeSRTs; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::vector<std::tuple<std::string, AnimationTimeSRT>> m_nodeSRTs;
    };
}

#endif // MODEL_ANIMATION_ASSEMBLER_H
