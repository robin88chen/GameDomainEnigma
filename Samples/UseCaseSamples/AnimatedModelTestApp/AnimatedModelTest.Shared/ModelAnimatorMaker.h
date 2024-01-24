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
#include <vector>
#include <string>
#include <memory>

class ModelAnimatorMaker
{
public:
    static void makeModelAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::string& mesh_node_name);
    //static std::shared_ptr<Enigma::Animators::ModelAnimationAsset> MakeModelAnimationAsset(
      //  const std::string& model_name, const std::vector<std::string>& mesh_node_names);
    //static std::shared_ptr<Enigma::Animators::ModelPrimitiveAnimator> MakeModelAnimator(
      //  const std::string& model_name, const std::shared_ptr<Enigma::Animators::ModelAnimationAsset>& anim);
};

#endif // _MODEL_ANIMATOR_MAKER_H
