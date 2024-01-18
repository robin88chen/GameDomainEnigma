/*********************************************************************
 * \file   AnimationAssetCommand.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef ANI_ANIMATION_ASSET_COMMAND_H
#define ANI_ANIMATION_ASSET_COMMAND_H

#include "Frameworks/Command.h"
#include <memory>

namespace Enigma::Animators
{
    class AnimationAssetPolicy;

    class BuildAnimationAsset : public Frameworks::ICommand
    {
    public:
        BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<AnimationAssetPolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<AnimationAssetPolicy> m_policy;
    };
}

#endif // _ANIMATION_ASSET_COMMAND_H
