/*********************************************************************
 * \file   ModelAnimatorBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_ANIMATOR_BUILDER_H
#define _MODEL_ANIMATOR_BUILDER_H

#include "Frameworks/EventSubscriber.h"
#include "GameEngine/FactoryDesc.h"
#include <memory>
#include <string>

namespace Enigma::Renderables
{
    class AnimationRepository;
    class ModelAnimatorPolicy;
    class ModelPrimitiveAnimator;

    class ModelAnimatorBuilder
    {
    public:
        ModelAnimatorBuilder(AnimationRepository* host);
        ModelAnimatorBuilder(const ModelAnimatorBuilder&) = delete;
        ModelAnimatorBuilder(ModelAnimatorBuilder&&) = delete;
        ~ModelAnimatorBuilder();
        ModelAnimatorBuilder& operator=(const ModelAnimatorBuilder&) = delete;
        ModelAnimatorBuilder& operator=(ModelAnimatorBuilder&&) = delete;

        void BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy);

    private:
        void LinkSkinMeshOperators();

        void OnAnimationAssetBuilt(const Frameworks::IEventPtr& e);
        void OnBuildAnimationAssetFailed(const Frameworks::IEventPtr& e);

    private:
        AnimationRepository* m_repository;
        std::shared_ptr<ModelAnimatorPolicy> m_policy;
        std::string m_assetName;

        Engine::FactoryDesc m_originalAssetDesc;
        std::shared_ptr<ModelPrimitiveAnimator> m_builtAnimator;

        Frameworks::EventSubscriberPtr m_onAnimationAssetBuilt;
        Frameworks::EventSubscriberPtr m_onBuildAnimationAssetFailed;
    };
}

#endif // _MODEL_ANIMATOR_BUILDER_H
