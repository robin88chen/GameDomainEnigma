/*********************************************************************
 * \file   SkinAnimationMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SKIN_ANIMATION_MAKER_H
#define _SKIN_ANIMATION_MAKER_H

#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Primitives/PrimitiveId.h"
#include "Animators/AnimationAsset.h"
#include "Animators/Animator.h"
#include <memory>
#include <string>
#include <vector>

class SkinAnimationMaker
{
public:
    static std::shared_ptr<Enigma::Animators::AnimationAsset> makeSkinMeshAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::vector<std::string>& bone_node_names);
    static std::shared_ptr<Enigma::Animators::Animator> makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& skin_mesh_id, const std::vector<std::string>& bone_node_names);
};

#endif // _SKIN_ANIMATION_MAKER_H
