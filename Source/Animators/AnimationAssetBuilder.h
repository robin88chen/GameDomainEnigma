/*********************************************************************
 * \file   AnimationAssetBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_BUILDER_H
#define _ANIMATION_ASSET_BUILDER_H

#include "Frameworks/ruid.h"
#include "GameEngine/GenericDto.h"
#include "Frameworks/EventSubscriber.h"
#include <memory>
#include <string>

namespace Enigma::Animators
{
    class AnimationAssetRepository;
    class AnimationAssetPolicy;

    class AnimationAssetBuilder
    {
    public:
        AnimationAssetBuilder(AnimationAssetRepository* host);
        AnimationAssetBuilder(const AnimationAssetBuilder&) = delete;
        AnimationAssetBuilder(AnimationAssetBuilder&&) = delete;
        ~AnimationAssetBuilder();
        AnimationAssetBuilder& operator=(const AnimationAssetBuilder&) = delete;
        AnimationAssetBuilder& operator=(AnimationAssetBuilder&&) = delete;

        void BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy);

    protected:
        void CreateFromDto(const std::string& name, const Engine::GenericDto& dto);

        void OnDtoAnimationAssetCreated(const Frameworks::IEventPtr& e);
        void OnDtoDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializeDtoFailed(const Frameworks::IEventPtr& e);

    private:
        AnimationAssetRepository* m_repository;
        std::shared_ptr<AnimationAssetPolicy> m_policy;

        Frameworks::Ruid m_ruidConstructing;
        Frameworks::Ruid m_ruidDeserializing;

        Frameworks::EventSubscriberPtr m_onDtoAnimationCreated;
        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeDtoFailed;
    };
}

#endif // _ANIMATION_ASSET_BUILDER_H
