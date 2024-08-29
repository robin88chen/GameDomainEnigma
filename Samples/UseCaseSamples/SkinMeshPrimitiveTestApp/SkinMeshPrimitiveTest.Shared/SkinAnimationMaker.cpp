#include "SkinAnimationMaker.h"
#include "Renderables/ModelAnimationAssembler.h"
#include "Renderables/ModelAnimatorAssembler.h"
#include "Renderables/ModelAnimationAsset.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/AnimationTimeSRTAssembler.h"
#include "Renderables/SkinOperatorAssembler.h"
#include "Animators/AnimationAssetQueries.h"
#include "Animators/AnimatorQueries.h"

using namespace Enigma::Animators;
using namespace Enigma::MathLib;
using namespace Enigma::Renderables;

std::shared_ptr<AnimationAsset> SkinAnimationMaker::makeSkinMeshAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::vector<std::string>& bone_node_names)
{
    if (auto anim = AnimationAsset::queryAnimationAsset(animation_id)) return anim;
    std::shared_ptr<AnimationTimeSRTAssembler> anim_srt_assembler = std::make_shared<AnimationTimeSRTAssembler>();
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(0.0f, Vector3(1.0f, 1.0f, 1.0f)));
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(1.0f, Vector3(0.5f, 0.5f, 0.5f)));
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(2.0f, Vector3(1.0f, 1.0f, 1.0f)));
    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(0.0f, Quaternion::IDENTITY));
    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(1.0f, Quaternion::IDENTITY));
    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(2.0f, Quaternion::IDENTITY));
    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(0.0f, Vector3(2.0f, 2.0f, 0.0f)));
    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(1.0f, Vector3(-1.2f, 1.2f, 0.0f)));
    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(2.0f, Vector3(2.0f, 2.0f, 0.0f)));
    ModelAnimationAssembler model_animation_assembler(animation_id);
    model_animation_assembler.nodeSRT(bone_node_names[bone_node_names.size() - 1], anim_srt_assembler);
    model_animation_assembler.asAsset(animation_id.name(), animation_id.name() + ".ani", "DataPath");
    return std::make_shared<RequestAnimationAssetConstitution>(animation_id, model_animation_assembler.assemble())->dispatch();
}

std::shared_ptr<Animator> SkinAnimationMaker::makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& skin_mesh_id, const std::vector<std::string>& bone_node_names)
{
    ModelAnimatorAssembler model_animator_assembler(animator_id);
    model_animator_assembler.animationAsset(animation_id);
    model_animator_assembler.controlledPrimitive(model_id);
    model_animator_assembler.asNative(animator_id.name() + ".animator@DataPath");
    std::shared_ptr<SkinOperatorAssembler> skin_operator_assembler = std::make_shared<SkinOperatorAssembler>();
    skin_operator_assembler->operatedSkin(skin_mesh_id);
    skin_operator_assembler->bones(bone_node_names);
    model_animator_assembler.addSkinOperator(skin_operator_assembler);
    return std::make_shared<RequestAnimatorConstitution>(animator_id, model_animator_assembler.assemble())->dispatch();
}
