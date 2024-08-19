/*********************************************************************
 * \file   ModelAnimatorFactoryMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MODEL_ANIMATOR_FACTORY_METHODS_H
#define MODEL_ANIMATOR_FACTORY_METHODS_H

#include "Animators/AnimatorId.h"
#include "GameEngine/GenericDto.h"
#include <memory>

namespace Enigma::Renderables
{
    class ModelPrimitiveAnimator;

    std::shared_ptr<ModelPrimitiveAnimator> createModelPrimitiveAnimator(const Animators::AnimatorId& id);
    std::shared_ptr<ModelPrimitiveAnimator> constituteModelPrimitiveAnimator(const Animators::AnimatorId& id, const Engine::GenericDto& dto);
}

#endif // MODEL_ANIMATOR_FACTORY_METHODS_H
