#include "SkinAnimationMaker.h"
#include "Renderables/ModelAnimatorDtos.h"
#include "Renderables/ModelAnimationDtos.h"
#include "Renderables/ModelAnimationAsset.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Animators/AnimationAssetDtos.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "Animators/AnimationAssetQueries.h"
#include "Animators/AnimatorQueries.h"

using namespace Enigma::Animators;
using namespace Enigma::MathLib;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

std::shared_ptr<AnimationAsset> SkinAnimationMaker::makeSkinMeshAnimationAsset(const Enigma::Animators::AnimationAssetId& animation_id, const std::vector<std::string>& bone_node_names)
{
    if (auto anim = AnimationAsset::queryAnimationAsset(animation_id)) return anim;

    AnimationTimeSRTDto anim_srt_dto;
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
    model_animation_asset_dto.meshNodeNames() = std::vector<std::string>{ bone_node_names[bone_node_names.size() - 1] };
    model_animation_asset_dto.timeSRTs().push_back(anim_srt_dto.toGenericDto());
    return std::make_shared<RequestAnimationAssetConstitution>(animation_id, model_animation_asset_dto.toGenericDto(), RequestAnimationAssetConstitution::PersistenceLevel::Store)->dispatch();
}

std::shared_ptr<Animator> SkinAnimationMaker::makeModelAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& skin_mesh_id, const std::vector<std::string>& bone_node_names)
{
    if (auto anim = Animator::queryAnimator(animator_id)) return anim;
    ModelAnimatorDto model_animator_dto;
    model_animator_dto.id() = animator_id;
    model_animator_dto.animationAssetId() = animation_id;
    model_animator_dto.controlledPrimitiveId() = model_id;
    model_animator_dto.factoryDesc() = Enigma::Engine::FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName()).ClaimAsNative(animator_id.name() + ".animator@DataPath");
    SkinOperatorDto operator_dto;
    operator_dto.boneNodeNames() = bone_node_names;
    operator_dto.skinMeshId() = skin_mesh_id;
    model_animator_dto.skinOperators().emplace_back(operator_dto.toGenericDto());

    return std::make_shared<RequestAnimatorConstitution>(animator_id, model_animator_dto.toGenericDto(), RequestAnimatorConstitution::PersistenceLevel::Store)->dispatch();
}
