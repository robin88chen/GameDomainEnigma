/*********************************************************************
 * \file   AnimatorInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef ANIMATOR_INSTALLING_POLICY_H
#define ANIMATOR_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"

namespace Enigma::Animators
{
    class AnimatorStoreMapper;
    class AnimationAssetStoreMapper;

    using error = std::error_code;

    class AnimatorInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        AnimatorInstallingPolicy(const std::shared_ptr<AnimatorStoreMapper>& animator_store, const std::shared_ptr<AnimationAssetStoreMapper>& animation_asset_store) : m_animatorStore(animator_store), m_animationAssetStore(animation_asset_store) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<AnimatorStoreMapper> m_animatorStore;
        std::shared_ptr<AnimationAssetStoreMapper> m_animationAssetStore;
    };
}

#endif // ANIMATOR_INSTALLING_POLICY_H
