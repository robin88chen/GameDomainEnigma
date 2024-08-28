#include "ModelAnimatorMaker.h"
#include "Renderables/ModelAnimatorAssembler.h"
#include "Renderables/ModelAnimationAssembler.h"
#include "Renderables/ModelAnimationAsset.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/AnimationTimeSRTAssembler.h"
#include "Animators/AnimationAssetQueries.h"
#include "Animators/AnimatorQueries.h"

using namespace Enigma::Animators;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;

std::shared_ptr<AnimationAsset> ModelAnimatorMaker::makeModelAnimationAsset(const AnimationAssetId& animation_id, const std::string& mesh_node_name)
{
    if (auto anim = AnimationAsset::queryAnimationAsset(animation_id)) return anim;

    std::shared_ptr<AnimationTimeSRTAssembler> anim_srt_assembler = std::make_shared<AnimationTimeSRTAssembler>();
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(0.0f, Vector3(1.0f, 1.0f, 1.0f)));
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(2.0f, Vector3(0.1f, 0.1f, 0.1f)));
    anim_srt_assembler->addScaleKey(AnimationTimeSRT::ScaleKey(4.0f, Vector3(1.0f, 1.0f, 1.0f)));

    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(0.0f, Quaternion::IDENTITY));
    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(2.0f, Quaternion::IDENTITY));
    anim_srt_assembler->addRotationKey(AnimationTimeSRT::RotationKey(4.0f, Quaternion::IDENTITY));

    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(0.0f, Vector3(2.0f, 2.0f, 0.0f)));
    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(2.0f, Vector3(-1.2f, 1.2f, 0.0f)));
    anim_srt_assembler->addTranslationKey(AnimationTimeSRT::TranslateKey(4.0f, Vector3(2.0f, 2.0f, 0.0f)));

    ModelAnimationAssembler model_animation_assembler(animation_id);
    model_animation_assembler.nodeSRT(mesh_node_name, anim_srt_assembler);
    model_animation_assembler.asAsset(animation_id.name(), animation_id.name() + ".ani", "DataPath");
    return std::make_shared<RequestAnimationAssetConstitution>(animation_id, model_animation_assembler.assemble())->dispatch();
}

std::shared_ptr<Animator> ModelAnimatorMaker::makeModelAnimator(const AnimatorId& animator_id, const AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id)
{
    if (auto anim = Animator::queryAnimator(animator_id)) return anim;

    ModelAnimatorAssembler model_animator_assembler(animator_id);
    model_animator_assembler.animationAsset(animation_id);
    model_animator_assembler.controlledPrimitive(model_id);
    model_animator_assembler.asNative(animator_id.name() + ".animator@DataPath");
    return std::make_shared<RequestAnimatorConstitution>(animator_id, model_animator_assembler.assemble())->dispatch();
}
