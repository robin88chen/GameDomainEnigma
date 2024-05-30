/*********************************************************************
 * \file   AnimationAssetRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_REPOSITORY_H
#define ANIMATION_ASSET_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "AnimationAsset.h"
#include "AnimatorFactoryDelegate.h"
#include <mutex>

namespace Enigma::Animators
{
    class AnimationAssetStoreMapper;
    class AnimationAssetFactory;

    class AnimationAssetRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        AnimationAssetRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<AnimationAssetStoreMapper>& store_mapper);
        AnimationAssetRepository(const AnimationAssetRepository&) = delete;
        AnimationAssetRepository(AnimationAssetRepository&&) = delete;
        virtual ~AnimationAssetRepository() override;
        AnimationAssetRepository& operator=(const AnimationAssetRepository&) = delete;
        AnimationAssetRepository& operator=(AnimationAssetRepository&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        void registerAnimationAssetFactory(const std::string& rtti_name, const AnimationAssetCreator& creator, const AnimationAssetConstitutor& constitutor);

        bool hasAnimationAsset(const AnimationAssetId& id);
        std::shared_ptr<AnimationAsset> queryAnimationAsset(const AnimationAssetId& id);
        void removeAnimationAsset(const AnimationAssetId& id);
        void putAnimationAsset(const AnimationAssetId& id, const std::shared_ptr<AnimationAsset>& asset);

    protected:
        void queryAnimationAsset(const Frameworks::IQueryPtr& q);
        void requestAnimationAssetCreation(const Frameworks::IQueryPtr& r);
        void requestAnimationAssetConstitution(const Frameworks::IQueryPtr& r);
        void removeAnimationAsset(const Frameworks::ICommandPtr& c);
        void putAnimationAsset(const Frameworks::ICommandPtr& c);

    protected:
        std::shared_ptr<AnimationAssetStoreMapper> m_storeMapper;
        AnimationAssetFactory* m_factory;
        std::unordered_map<AnimationAssetId, std::shared_ptr<AnimationAsset>, AnimationAssetId::hash> m_animationAssets;
        std::recursive_mutex m_animationAssetLock;

        Frameworks::QuerySubscriberPtr m_queryAnimationAsset;
        Frameworks::QuerySubscriberPtr m_requestAnimationAssetCreation;
        Frameworks::QuerySubscriberPtr m_requestAnimationAssetConstitution;
        Frameworks::CommandSubscriberPtr m_removeAnimationAsset;
        Frameworks::CommandSubscriberPtr m_putAnimationAsset;
    };
}

#endif // ANIMATION_ASSET_REPOSITORY_H
