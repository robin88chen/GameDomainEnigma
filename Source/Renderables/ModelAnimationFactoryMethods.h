/*********************************************************************
 * \file   ModelAnimationFactoryMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MODEL_ANIMATION_FACTORY_METHODS_H
#define MODEL_ANIMATION_FACTORY_METHODS_H

#include "GameEngine/GenericDto.h"
#include "Animators/AnimationAssetId.h"

namespace Enigma::Renderables
{
    class ModelAnimationAsset;

    std::shared_ptr<ModelAnimationAsset> createModelAnimationAsset(const Animators::AnimationAssetId& id);
    std::shared_ptr<ModelAnimationAsset> constituteModelAnimationAsset(const Animators::AnimationAssetId& id, const Engine::GenericDto& dto);
}

#endif // MODEL_ANIMATION_FACTORY_METHODS_H
