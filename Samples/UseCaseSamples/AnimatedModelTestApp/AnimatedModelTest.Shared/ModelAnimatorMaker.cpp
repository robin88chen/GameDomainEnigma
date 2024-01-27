#include "ModelAnimatorMaker.h"
#include "Renderables/ModelAnimatorDtos.h"
#include "Renderables/ModelAnimationDtos.h"
#include "Renderables/ModelAnimationAsset.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Animators/AnimationAssetCommands.h"
#include "Animators/AnimatorCommands.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::Animators;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;

void ModelAnimatorMaker::makeModelAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::string& mesh_node_name)
{
    AnimationTimeSRTDto anim_srt_dto;
    AnimationTimeSRT::ScaleKeyVector scale_key;
    scale_key.push_back(AnimationTimeSRT::ScaleKey(0.0f, Vector3(1.0f, 1.0f, 1.0f)));
    scale_key.push_back(AnimationTimeSRT::ScaleKey(2.0f, Vector3(0.1f, 0.1f, 0.1f)));
    scale_key.push_back(AnimationTimeSRT::ScaleKey(4.0f, Vector3(1.0f, 1.0f, 1.0f)));
    AnimationTimeSRT::RotationKeyVector rot_key;
    rot_key.push_back(AnimationTimeSRT::RotationKey(0.0f, Quaternion::IDENTITY));
    rot_key.push_back(AnimationTimeSRT::RotationKey(2.0f, Quaternion::IDENTITY));
    rot_key.push_back(AnimationTimeSRT::RotationKey(4.0f, Quaternion::IDENTITY));
    AnimationTimeSRT::TranslateKeyVector trans_key;
    trans_key.push_back(AnimationTimeSRT::TranslateKey(0.0f, Vector3(2.0f, 2.0f, 0.0f)));
    trans_key.push_back(AnimationTimeSRT::TranslateKey(2.0f, Vector3(-1.2f, 1.2f, 0.0f)));
    trans_key.push_back(AnimationTimeSRT::TranslateKey(4.0f, Vector3(2.0f, 2.0f, 0.0f)));
    for (auto& key : scale_key)
    {
        auto values = key.values();
        anim_srt_dto.scaleTimeKeys().insert(anim_srt_dto.scaleTimeKeys().end(), values.begin(), values.end());
    }
    for (auto& key : rot_key)
    {
        auto values = key.values();
        anim_srt_dto.rotateTimeKeys().insert(anim_srt_dto.rotateTimeKeys().end(), values.begin(), values.end());
    }
    for (auto& key : trans_key)
    {
        auto values = key.values();
        anim_srt_dto.translateTimeKeys().insert(anim_srt_dto.translateTimeKeys().end(), values.begin(), values.end());
    }

    ModelAnimationAssetDto model_animation_asset_dto;
    model_animation_asset_dto.id() = animation_id;
    model_animation_asset_dto.factoryDesc() = Enigma::Engine::FactoryDesc(ModelAnimationAsset::TYPE_RTTI.getName()).ClaimAsResourceAsset(animation_id.name(), animation_id.name() + ".ani", "DataPath");
    model_animation_asset_dto.meshNodeNames() = std::vector<std::string>{ mesh_node_name };
    model_animation_asset_dto.timeSRTs().push_back(anim_srt_dto.toGenericDto());
    Enigma::Frameworks::CommandBus::post(std::make_shared<ConstituteAnimationAsset>(animation_id, model_animation_asset_dto.toGenericDto()));
}

void ModelAnimatorMaker::makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id)
{
    ModelAnimatorDto model_animator_dto;
    model_animator_dto.id() = animator_id;
    model_animator_dto.animationAssetId() = animation_id;
    model_animator_dto.controlledPrimitiveId() = model_id;
    model_animator_dto.factoryDesc() = Enigma::Engine::FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName()).ClaimAsNative(animator_id.name() + ".animator@DataPath");
    Enigma::Frameworks::CommandBus::post(std::make_shared<ConstituteAnimator>(animator_id, model_animator_dto.toGenericDto()));
}
