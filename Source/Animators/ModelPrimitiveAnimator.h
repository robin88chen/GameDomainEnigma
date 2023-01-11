/*********************************************************************
 * \file   ModelPrimitiveAnimator.h
 * \brief  model primitive animator, value object, use shared_ptr
 * !!! 可以複製，不能搬! 被搬走的還在變化中怎麼辦??
 * !!! 控制的物件不會被深層複製, 讓 Builder 來處理
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_ANIMATOR_H
#define _MODEL_PRIMITIVE_ANIMATOR_H

#include "GameEngine/Animator.h"
#include "AnimationClip.h"
#include "Renderer/ModelPrimitive.h"
#include "AnimatorDtos.h"
#include <optional>
#include <memory>

namespace Enigma::Animators
{
    class ModelAnimationAsset;

    class ModelPrimitiveAnimator : public Engine::Animator
    {
        DECLARE_EN_RTTI
    public:
        ModelPrimitiveAnimator();
        ModelPrimitiveAnimator(const ModelPrimitiveAnimator& ani);
        ModelPrimitiveAnimator(ModelPrimitiveAnimator&& ani) = delete;
        ~ModelPrimitiveAnimator() override;
        ModelPrimitiveAnimator& operator=(const ModelPrimitiveAnimator& ani);
        ModelPrimitiveAnimator& operator=(ModelPrimitiveAnimator&& ani) = delete;

        ModelAnimatorDto SerializeDto();
        virtual HasUpdated Update(Frameworks::Timer* timer) override;
        virtual void Reset() override;

        void SetControlledModel(const std::shared_ptr<Renderer::ModelPrimitive>& model) { m_controlledPrimitive = model; }

        /** link animation set, then re-calculate mapping */
        void LinkAnimationAsset(const std::shared_ptr<ModelAnimationAsset>& anim_asset);
        /** calculate mesh node mapping */
        virtual void CalculateMeshNodeMapping();

        //todo : skin mesh

        /** get animation key set */
        const std::shared_ptr<ModelAnimationAsset>& GetAnimationAsset() { return m_animationAsset; };

        /** play animation */
        virtual void PlayAnimation(const AnimationClip& clip);
        /** fade in animation */
        virtual void FadeInAnimation(float fading_time, const AnimationClip& clip);
        /** stop animation */
        virtual void StopAnimation();

    protected:
        bool TimeValueUpdate();

        bool UpdateMeshNodeTransform();
        bool UpdateMeshNodeTransformWithFading();

    protected:
        struct MeshNodeMappingData
        {
            std::optional<unsigned> m_nodeIndexInModel;   ///< mesh node index in controlled model
            std::optional<unsigned> m_nodeIndexInAnimation; ///< mesh node index in animation key set
        };
        typedef std::vector<MeshNodeMappingData> MeshNodeMappingArray;

    protected:
        std::weak_ptr<Renderer::ModelPrimitive> m_controlledPrimitive; ///< 控制的物件，不會跟著深層複製

        std::shared_ptr<ModelAnimationAsset> m_animationAsset;
        MeshNodeMappingArray m_meshNodeMapping;

        AnimationClip m_currentAnimClip;
        AnimationClip m_fadeInAnimClip;

        float m_remainFadingTime;   ///< fade weight = remain fading time / fading time
        float m_fadingTime;

        bool m_isFading;

        bool m_isOnPlay;

        //todo: skin mesh
        //typedef std::vector<SkinAnimOperatorPtr> SkinAnimationOperatorVector;
        //SkinAnimationOperatorVector m_skinAnimOperators;
    };
    using ModelPrimitiveAnimatorPtr = std::shared_ptr<ModelPrimitiveAnimator>;
}

#endif // _MODEL_PRIMITIVE_ANIMATOR_H
