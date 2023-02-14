/*********************************************************************
 * \file   SkinAnimationMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SKIN_ANIMATION_MAKER_H
#define _SKIN_ANIMATION_MAKER_H

#include "Animators/ModelAnimationAsset.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "Animators/AnimatorDtos.h"
#include <memory>
#include <string>
#include <vector>

class SkinAnimationMaker
{
public:
    static std::shared_ptr<Enigma::Animators::ModelAnimationAsset> MakeSkinAnimationAsset(
        const std::string& model_name, const std::vector<std::string>& bone_node_names);
    static Enigma::Animators::ModelAnimatorDto MakeModelAnimatorDto(
        const std::shared_ptr<Enigma::Animators::ModelAnimationAsset>& anim, const std::string& skinmesh_name, const std::vector<std::string>& bone_node_names);
};

#endif // _SKIN_ANIMATION_MAKER_H
