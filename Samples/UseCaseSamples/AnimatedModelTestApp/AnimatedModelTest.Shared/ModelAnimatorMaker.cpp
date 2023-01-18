#include "ModelAnimatorMaker.h"
#include "Animators/ModelAnimationAsset.h"
#include "Animators/AnimationTimeSRT.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include <memory>

using namespace Enigma::Animators;
using namespace Enigma::MathLib;

std::shared_ptr<ModelAnimationAsset> ModelAnimatorMaker::MakeModelAnimationAsset(
    const std::string& model_name, const std::vector<std::string>& mesh_node_names)
{
    std::string anim_name = model_name + "_anim";
    std::shared_ptr<ModelAnimationAsset> animation_asset = std::make_shared<ModelAnimationAsset>(anim_name);
    AnimationTimeSRT::ScaleKeyVector scale_key;
    scale_key.push_back(AnimationTimeSRT::ScaleKey(0.0f, Vector3(1.0f, 1.0f, 1.0f)));
    scale_key.push_back(AnimationTimeSRT::ScaleKey(1.0f, Vector3(0.5f, 0.5f, 0.5f)));
    scale_key.push_back(AnimationTimeSRT::ScaleKey(2.0f, Vector3(1.0f, 1.0f, 1.0f)));
    AnimationTimeSRT::RotationKeyVector rot_key;
    rot_key.push_back(AnimationTimeSRT::RotationKey(0.0f, Quaternion::IDENTITY));
    rot_key.push_back(AnimationTimeSRT::RotationKey(1.0f, Quaternion::IDENTITY));
    rot_key.push_back(AnimationTimeSRT::RotationKey(2.0f, Quaternion::IDENTITY));
    AnimationTimeSRT::TranslateKeyVector trans_key;
    trans_key.push_back(AnimationTimeSRT::TranslateKey(0.0f, Vector3(2.0f, 2.0f, 0.0f)));
    trans_key.push_back(AnimationTimeSRT::TranslateKey(1.0f, Vector3(-1.2f, 1.2f, 0.0f)));
    trans_key.push_back(AnimationTimeSRT::TranslateKey(2.0f, Vector3(2.0f, 2.0f, 0.0f)));
    AnimationTimeSRT anim_srt;
    anim_srt.SetScaleKeyVector(scale_key);
    anim_srt.SetRotationKeyVector(rot_key);
    anim_srt.SetTranslateKeyVector(trans_key);
    animation_asset->AddMeshNodeTimeSRTData(mesh_node_names[mesh_node_names.size() - 1], anim_srt);
    return animation_asset;
}

std::shared_ptr<ModelPrimitiveAnimator> ModelAnimatorMaker::MakeModelAnimator(
    const std::string& model_name, const std::shared_ptr<ModelAnimationAsset>& anim)
{
    std::shared_ptr<ModelPrimitiveAnimator> animator = std::make_shared<ModelPrimitiveAnimator>();
    animator->LinkAnimationAsset(anim);
    return animator;
}
