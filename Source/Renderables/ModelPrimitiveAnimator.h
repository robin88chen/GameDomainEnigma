﻿/*********************************************************************
 * \file   ModelPrimitiveAnimator.h
 * \brief  model primitive animator, value object, use shared_ptr
 * !!! 可以複製，不能搬! 被搬走的還在變化中怎麼辦??
 * !!! 控制的物件不會被深層複製, 讓 Builder 來處理
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_ANIMATOR_H
#define _MODEL_PRIMITIVE_ANIMATOR_H

#include "Animators/Animator.h"
#include "Animators/AnimatorAssembler.h"
#include "AnimationClip.h"
#include "ModelPrimitive.h"
#include "SkinAnimationOperator.h"
#include "MeshNodeTree.h"
#include <optional>
#include <memory>

namespace Enigma::Renderables
{
    class ModelAnimationAsset;

    class ModelPrimitiveAnimator : public Animators::Animator
    {
        DECLARE_EN_RTTI
    public:
        ModelPrimitiveAnimator(const Animators::AnimatorId& id);
        ModelPrimitiveAnimator(const ModelPrimitiveAnimator& ani) = delete;
        ModelPrimitiveAnimator(ModelPrimitiveAnimator&& ani) = delete;
        ~ModelPrimitiveAnimator() override;
        ModelPrimitiveAnimator& operator=(const ModelPrimitiveAnimator& ani) = delete;
        ModelPrimitiveAnimator& operator=(ModelPrimitiveAnimator&& ani) = delete;

        virtual std::shared_ptr<Animators::AnimatorAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<Animators::AnimatorAssembler>& assembler) const override;
        virtual std::shared_ptr<Animators::AnimatorDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<Animators::AnimatorDisassembler>& disassembler) override;

        virtual HasUpdated update(const std::unique_ptr<Frameworks::Timer>& timer) override;
        virtual void reset() override;

        void onAttachingMeshNodeTree(const Primitives::PrimitiveId& model_id, const MeshNodeTree& mesh_node_tree);
        void onDetachingMeshNodeTree();

        /** get skin mesh animation operator,
                model 中有多個 skin mesh, 每個 skin mesh 有一個 operator*/
        const SkinAnimationOperator& getSkinAnimOperator(unsigned int index);
        unsigned getSkinAnimationOperatorCount() const { return static_cast<unsigned>(m_skinAnimOperators.size()); };

        /** get animation key set */
        const std::shared_ptr<ModelAnimationAsset>& getAnimationAsset() { return m_animationAsset; };

        /** play animation */
        virtual void playAnimation(const AnimationClip& clip);
        /** fade in animation */
        virtual void fadeInAnimation(float fading_time, const AnimationClip& clip);
        /** stop animation */
        virtual void stopAnimation();

    protected:
        bool updateTimeValue();

        bool updateMeshNodeTransform();
        bool updateMeshNodeTransformWithFading();

        /** calculate mesh node mapping */
        void calculateMeshNodeMapping(const MeshNodeTree& mesh_node_tree);

        std::shared_ptr<Renderables::ModelPrimitive> cacheControlledModel();

    protected:
        struct MeshNodeMappingData
        {
            std::optional<unsigned> m_nodeIndexInModel;   ///< mesh node index in controlled model
            std::optional<unsigned> m_nodeIndexInAnimation; ///< mesh node index in animation key set
        };
        typedef std::vector<MeshNodeMappingData> MeshNodeMappingArray;

    protected:
        std::weak_ptr<Renderables::ModelPrimitive> m_controlledModel; ///< 控制的物件，不會跟著深層複製

        std::shared_ptr<ModelAnimationAsset> m_animationAsset;
        MeshNodeMappingArray m_meshNodeMapping;

        AnimationClip m_currentAnimClip;
        AnimationClip m_fadeInAnimClip;

        float m_remainFadingTime;   ///< fade weight = remain fading time / fading time
        float m_fadingTime;

        bool m_isFading;

        bool m_isOnPlay;

        std::vector<SkinAnimationOperator> m_skinAnimOperators;
    };
}

#endif // _MODEL_PRIMITIVE_ANIMATOR_H
