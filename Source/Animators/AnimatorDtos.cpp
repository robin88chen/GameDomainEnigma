#include "AnimatorDtos.h"
#include "AnimationAssetPolicies.h"
#include "AnimatorPolicies.h"
#include "ModelAnimationAsset.h"
#include "ModelPrimitiveAnimator.h"

using namespace Enigma::Animators;
using namespace Enigma::Engine;

static std::string TOKEN_ANIMATION_NAME = "AnimationName";
static std::string TOKEN_ASSET_OBJECT = "AssetObject";
static std::string TOKEN_ANIMATION_FACTORY = "AnimationFactory";
static std::string TOKEN_SKIN_OPERATORS = "SkinOperators";
static std::string TOKEN_SKIN_MESH_NAME = "SkinMeshName";
static std::string TOKEN_SKIN_MESH_NODE_NAME = "SkinMeshNodeName";
static std::string TOKEN_BONE_NODE_NAMES = "BoneNodeNames";
static std::string TOKEN_NODE_OFFSETS = "NodeOffsets";

ModelAnimatorDto::ModelAnimatorDto() : m_assetFactory(ModelAnimationAsset::TYPE_RTTI.getName()), m_factoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName())
{
}

ModelAnimatorDto ModelAnimatorDto::fromGenericDto(const GenericDto& dto)
{
    ModelAnimatorDto model;
    model.factoryDesc() = dto.GetRtti();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_ASSET_OBJECT)) model.AnimationAssetDto() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_ANIMATION_NAME)) model.AssetName() = v.value();
    if (auto v = dto.TryGetValue<FactoryDesc>(TOKEN_ANIMATION_FACTORY)) model.AssetFactoryDesc() = v.value();
    if (auto v = dto.TryGetValue<GenericDtoCollection>(TOKEN_SKIN_OPERATORS)) model.SkinOperators() = v.value();
    return model;
}

GenericDto ModelAnimatorDto::toGenericDto()
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    if (m_animationAssetDto) dto.AddOrUpdate(TOKEN_ASSET_OBJECT, m_animationAssetDto.value());
    dto.AddOrUpdate(TOKEN_ANIMATION_NAME, m_assetName);
    dto.AddOrUpdate(TOKEN_ANIMATION_FACTORY, m_assetFactory);
    dto.AddOrUpdate(TOKEN_SKIN_OPERATORS, m_skinOperators);
    return dto;
}

std::shared_ptr<ModelAnimatorPolicy> ModelAnimatorDto::ConvertToPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled,
    const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    std::shared_ptr<AnimationAssetPolicy> asset_policy;
    std::shared_ptr<ModelAnimatorPolicy> policy;
    if (m_animationAssetDto)
    {
        asset_policy = std::make_shared<AnimationAssetPolicy>(m_assetName, m_animationAssetDto.value());
        policy = std::make_shared<ModelAnimatorPolicy>(controlled, asset_policy);
        policy->AssetFactoryDesc() = m_animationAssetDto.value().GetRtti();
    }
    else
    {
        asset_policy = std::make_shared<AnimationAssetPolicy>(m_assetName, m_assetFactory.GetResourceFilename(), deserializer);
        policy = std::make_shared<ModelAnimatorPolicy>(controlled, asset_policy);
        policy->AssetFactoryDesc() = m_assetFactory;
    }
    for (auto& op : m_skinOperators)
    {
        policy->SkinOperators().emplace_back(SkinOperatorDto::fromGenericDto(op));
    }
    return policy;
}

SkinOperatorDto::SkinOperatorDto() : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
}

SkinOperatorDto SkinOperatorDto::fromGenericDto(const Engine::GenericDto& dto)
{
    SkinOperatorDto op;
    op.factoryDesc() = dto.GetRtti();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_SKIN_MESH_NAME)) op.SkinMeshName() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_SKIN_MESH_NODE_NAME)) op.SkinMeshNodeName() = v.value();
    if (auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_BONE_NODE_NAMES)) op.BoneNodeNames() = v.value();
    if (auto v = dto.TryGetValue<std::vector<MathLib::Matrix4>>(TOKEN_NODE_OFFSETS)) op.NodeOffsets() = v.value();
    return op;
}

GenericDto SkinOperatorDto::toGenericDto()
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    if (m_skinMeshName) dto.AddOrUpdate(TOKEN_SKIN_MESH_NAME, m_skinMeshName.value());
    if (m_skinMeshNodeName) dto.AddOrUpdate(TOKEN_SKIN_MESH_NODE_NAME, m_skinMeshNodeName.value());
    dto.AddOrUpdate(TOKEN_BONE_NODE_NAMES, m_boneNodeNames);
    if (m_nodeOffsets) dto.AddOrUpdate(TOKEN_NODE_OFFSETS, m_nodeOffsets);
    return dto;
}
