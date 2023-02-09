#include "SkinAnimationMaker.h"
#include "GameEngine/GenericDto.h"
#include "Animators/ModelAnimationAsset.h"
#include "Animators/AnimationAssetDtos.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/ExtentTypesDefine.h"

using namespace Enigma::Animators;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

std::shared_ptr<ModelAnimationAsset> SkinAnimationMaker::MakeSkinAnimationAsset(
    const std::string& model_name, const std::vector<std::string>& bone_node_names)
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
    animation_asset->AddMeshNodeTimeSRTData(bone_node_names[bone_node_names.size() - 1], anim_srt);

    GenericDto dto = animation_asset->SerializeDto().ToGenericDto();
    FactoryDesc desc(ModelAnimationAsset::TYPE_RTTI.GetName());
    desc.ClaimAsResourceAsset(anim_name, anim_name + ".ani", "DataPath");
    dto.AddRtti(desc);
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto>{dto});
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(anim_name + ".ani@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return animation_asset;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);

    return animation_asset;
}

ModelAnimatorDto SkinAnimationMaker::MakeModelAnimatorDto(
    const std::shared_ptr<ModelAnimationAsset>& anim, const std::string& skinmesh_name, const std::vector<std::string>& bone_node_names)
{
    std::shared_ptr<ModelPrimitiveAnimator> animator = std::make_shared<ModelPrimitiveAnimator>();
    animator->LinkAnimationAsset(anim);
    auto dto = animator->SerializeDto();
    SkinOperatorDto operator_dto;
    operator_dto.BoneNodeNames() = bone_node_names;
    operator_dto.SkinMeshName() = skinmesh_name;
    dto.SkinOperators().emplace_back(operator_dto.ToGenericDto());
    return dto;
}

