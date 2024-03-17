#include "SkinAnimationMaker.h"
#include "Renderables/ModelAnimationAssembler.h"
#include "Renderables/ModelAnimatorAssembler.h"
#include "Renderables/ModelAnimationAsset.h"
#include "Renderables/ModelPrimitiveAnimator.h"

using namespace Enigma::Animators;
using namespace Enigma::MathLib;
using namespace Enigma::Renderables;

std::shared_ptr<AnimationAsset> SkinAnimationMaker::makeSkinMeshAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::vector<std::string>& bone_node_names)
{
    if (auto anim = AnimationAsset::queryAnimationAsset(animation_id)) return anim;
    AnimationTimeSRTAssembler anim_srt_assembler;
    anim_srt_assembler.scaleKey(AnimationTimeSRT::ScaleKey(0.0f, Vector3(1.0f, 1.0f, 1.0f)));
    anim_srt_assembler.scaleKey(AnimationTimeSRT::ScaleKey(1.0f, Vector3(0.5f, 0.5f, 0.5f)));
    anim_srt_assembler.scaleKey(AnimationTimeSRT::ScaleKey(2.0f, Vector3(1.0f, 1.0f, 1.0f)));
    anim_srt_assembler.rotationKey(AnimationTimeSRT::RotationKey(0.0f, Quaternion::IDENTITY));
    anim_srt_assembler.rotationKey(AnimationTimeSRT::RotationKey(1.0f, Quaternion::IDENTITY));
    anim_srt_assembler.rotationKey(AnimationTimeSRT::RotationKey(2.0f, Quaternion::IDENTITY));
    anim_srt_assembler.translationKey(AnimationTimeSRT::TranslateKey(0.0f, Vector3(2.0f, 2.0f, 0.0f)));
    anim_srt_assembler.translationKey(AnimationTimeSRT::TranslateKey(1.0f, Vector3(-1.2f, 1.2f, 0.0f)));
    anim_srt_assembler.translationKey(AnimationTimeSRT::TranslateKey(2.0f, Vector3(2.0f, 2.0f, 0.0f)));
    return ModelAnimationAssembler(animation_id).nodeSRT(bone_node_names[bone_node_names.size() - 1], anim_srt_assembler).asAsset(animation_id.name(), animation_id.name() + ".ani", "DataPath").constitute(PersistenceLevel::Store);
}

std::shared_ptr<Animator> SkinAnimationMaker::makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& skin_mesh_id, const std::vector<std::string>& bone_node_names)
{
    if (auto anim = Animator::queryAnimator(animator_id)) return anim;
    return ModelAnimatorAssembler(animator_id).animationAsset(animation_id).controlledPrimitive(model_id).skinOperator(SkinOperatorAssembler().operatedSkin(skin_mesh_id).bones(bone_node_names)).asNative(animator_id.name() + ".animator@DataPath").constitute(PersistenceLevel::Store);
}

