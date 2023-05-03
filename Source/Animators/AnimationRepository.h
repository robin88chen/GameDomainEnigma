/*********************************************************************
 * \file   AnimationRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_REPOSITORY_H
#define _ANIMATION_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/GenericDto.h"
#include <memory>
#include <mutex>
#include <queue>

namespace Enigma::Animators
{
    using error = std::error_code;
    class AnimationAsset;
    class AnimationAssetPolicy;
    class ModelAnimatorPolicy;
    class AnimationAssetBuilder;
    class ModelAnimatorBuilder;

    class AnimationRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI
    public:
        AnimationRepository(Frameworks::ServiceManager* manager);
        AnimationRepository(const AnimationRepository&) = delete;
        AnimationRepository(AnimationRepository&&) = delete;
        virtual ~AnimationRepository() override;
        AnimationRepository& operator=(const AnimationRepository&) = delete;
        AnimationRepository& operator=(AnimationRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult OnTick() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        bool HasAnimationAsset(const std::string& rtti_name, const std::string& name);
        std::shared_ptr<AnimationAsset> QueryAnimationAsset(const std::string& rtti_name, const std::string& name);

        error BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy);
        error BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy);

    private:
        /// true : current building
        bool PopAssetBuilding();
        bool PopAnimatorBuilding();

        std::string MakeAssetKey(const std::string& rtti_name, const std::string& name);

        void ModelAnimationAssetFactory(const Engine::GenericDto& dto);

        void DoBuildingAnimationAsset(const Frameworks::ICommandPtr& c);
        void DoBuildingModelAnimator(const Frameworks::ICommandPtr& c);
        void OnAnimationAssetBuilt(const Frameworks::IEventPtr& e);
        void OnBuildAnimationAssetFailed(const Frameworks::IEventPtr& e);
        void OnAnimatorBuilt(const Frameworks::IEventPtr& e);
        void OnBuildAnimatorFailed(const Frameworks::IEventPtr& e);
    private:
        std::unordered_map<std::string, std::weak_ptr<AnimationAsset>> m_assets;
        std::recursive_mutex m_assetsLock;

        std::unique_ptr<AnimationAssetBuilder> m_assetBuilder;
        std::unique_ptr<ModelAnimatorBuilder> m_animatorBuilder;

        std::queue<std::shared_ptr<AnimationAssetPolicy>> m_assetPolicies;
        bool m_isAssetCurrentBuilding;
        std::string m_buildingAssetName;
        std::mutex m_assetPoliciesLock;

        std::queue<std::shared_ptr<ModelAnimatorPolicy>> m_animatorPolicies;
        bool m_isAnimatorCurrentBuilding;
        Frameworks::Ruid m_constructingAnimatorRuid;
        std::mutex m_animatorPoliciesLock;

        Frameworks::CommandSubscriberPtr m_doBuildingAnimationAsset;
        Frameworks::CommandSubscriberPtr m_doBuildingModelAnimator;
        Frameworks::EventSubscriberPtr m_onAnimationAssetBuilt;
        Frameworks::EventSubscriberPtr m_onBuildAnimationAssetFailed;
        Frameworks::EventSubscriberPtr m_onAnimatorBuilt;
        Frameworks::EventSubscriberPtr m_onBuildAnimatorFailed;
    };
}

#endif // _ANIMATION_REPOSITORY_H
