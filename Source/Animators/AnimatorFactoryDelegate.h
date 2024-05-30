/*****************************************************************
 * \file   AnimatorFactoryDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef ANIMATOR_FACTORY_DELEGATE_H
#define ANIMATOR_FACTORY_DELEGATE_H

#include <functional>
#include <memory>

namespace Enigma::Engine
{
    class GenericDto;
}

namespace Enigma::Animators
{
    class AnimationAsset;
    class AnimationAssetId;

    using AnimationAssetCreator = std::function<std::shared_ptr<AnimationAsset>(const AnimationAssetId& id)>;
    using AnimationAssetConstitutor = std::function<std::shared_ptr<AnimationAsset>(const AnimationAssetId& id, const Engine::GenericDto& dto)>;

    class Animator;
    class AnimatorId;

    using AnimatorCreator = std::function<std::shared_ptr<Animator>(const AnimatorId& id)>;
    using AnimatorConstitutor = std::function<std::shared_ptr<Animator>(const AnimatorId& id, const Engine::GenericDto& dto)>;
}

#endif // ANIMATOR_FACTORY_DELEGATE_H
