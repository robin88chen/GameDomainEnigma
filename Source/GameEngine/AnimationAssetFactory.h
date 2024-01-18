/*********************************************************************
 * \file   AnimationAssetFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_FACTORY_H
#define ANIMATION_ASSET_FACTORY_H

#include "AnimationAsset.h"
#include "GenericDto.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::Engine
{
    using AnimationAssetCreator = std::function<std::shared_ptr<AnimationAsset>(const AnimationAssetId& id)>;
    using AnimationAssetConstitutor = std::function<std::shared_ptr<AnimationAsset>(const AnimationAssetId& id, const GenericDto& dto)>;

    class AnimationAssetFactory
    {
    public:
        AnimationAssetFactory();
        ~AnimationAssetFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<AnimationAsset> create(const AnimationAssetId& id, const Frameworks::Rtti& rtti);
        std::shared_ptr<AnimationAsset> constitute(const AnimationAssetId& id, const Engine::GenericDto& dto, bool is_persisted);

        void registerAnimationAssetFactory(const std::string& rtti_name, const AnimationAssetCreator& creator, const AnimationAssetConstitutor& constitutor);
        void unregisterAnimationAssetFactory(const std::string& rtti_name);

    private:
        void registerAnimationAssetFactory(const Frameworks::ICommandPtr& c);
        void unregisterAnimationAssetFactory(const Frameworks::ICommandPtr& c);
        void createAnimationAsset(const Frameworks::ICommandPtr& c);
        void constituteAnimationAsset(const Frameworks::ICommandPtr& c);

    private:
        std::unordered_map<std::string, AnimationAssetCreator> m_creators; // rtti name -> creator
        std::unordered_map<std::string, AnimationAssetConstitutor> m_constitutors; // rtti name -> constitutor

        Frameworks::CommandSubscriberPtr m_registerAnimationAssetFactory;
        Frameworks::CommandSubscriberPtr m_unregisterAnimationAssetFactory;
        Frameworks::CommandSubscriberPtr m_createAnimationAsset;
        Frameworks::CommandSubscriberPtr m_constituteAnimationAsset;
    };
}

#endif // ANIMATION_ASSET_FACTORY_H
