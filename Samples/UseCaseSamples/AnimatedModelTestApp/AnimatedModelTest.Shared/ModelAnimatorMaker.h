/*********************************************************************
 * \file   ModelAnimatorMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_ANIMATOR_MAKER_H
#define _MODEL_ANIMATOR_MAKER_H

#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Animators/AnimationAsset.h"
#include "Animators/Animator.h"
#include "Primitives/PrimitiveId.h"
#include <vector>
#include <string>
#include <memory>

class ModelAnimatorMaker
{
public:
    static std::shared_ptr<Enigma::Animators::AnimationAsset> makeModelAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::string& mesh_node_name);
    static std::shared_ptr<Enigma::Animators::Animator> makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id);
};

#endif // _MODEL_ANIMATOR_MAKER_H
