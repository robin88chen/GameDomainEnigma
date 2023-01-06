/*********************************************************************
 * \file   AnimationAssetRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_REPOSITORY_H
#define _ANIMATION_ASSET_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/GenericDto.h"
#include <memory>
#include <mutex>
#include <queue>

namespace Enigma::Animators
{
    using error = std::error_code;
    class AnimationAsset;
    class AnimationAssetPolicy;
    class AnimationAssetBuilder;

    class AnimationAssetRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI
    public:
        AnimationAssetRepository(Frameworks::ServiceManager* manager);
        AnimationAssetRepository(const AnimationAssetRepository&) = delete;
        AnimationAssetRepository(AnimationAssetRepository&&) = delete;
        virtual ~AnimationAssetRepository() override;
        AnimationAssetRepository& operator=(const AnimationAssetRepository&) = delete;
        AnimationAssetRepository& operator=(AnimationAssetRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult OnTick() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        bool HasAnimationAsset(const std::string& rtti_name, const std::string& name);
        std::shared_ptr<AnimationAsset> QueryAnimationAsset(const std::string& rtti_name, const std::string& name);

        error BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy);

    private:
        std::string MakeAssetKey(const std::string& rtti_name, const std::string& name);

        void ModelAnimationAssetFactory(const Engine::GenericDto& dto);

        void DoBuildingAnimationAsset(const Frameworks::ICommandPtr& c);
    private:
        std::unordered_map<std::string, std::weak_ptr<AnimationAsset>> m_assets;
        std::recursive_mutex m_assetsLock;

        std::unique_ptr<AnimationAssetBuilder> m_builder;

        std::queue<std::shared_ptr<AnimationAssetPolicy>> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;

        Frameworks::CommandSubscriberPtr m_doBuildingAnimationAsset;
    };
}

#endif // _ANIMATION_ASSET_REPOSITORY_H
