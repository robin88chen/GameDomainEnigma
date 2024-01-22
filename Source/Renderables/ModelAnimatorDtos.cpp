#include "ModelAnimatorDtos.h"
#include "ModelAnimationAsset.h"
#include "ModelPrimitiveAnimator.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_CONTROLLED_PRIMITIVE_ID = "ControlledPrimitiveId";
static std::string TOKEN_ASSET_ID = "AssetId";
static std::string TOKEN_SKIN_OPERATORS = "SkinOperators";
static std::string TOKEN_SKIN_MESH_ID = "SkinMeshId";
static std::string TOKEN_SKIN_MESH_NODE_NAME = "SkinMeshNodeName";
static std::string TOKEN_BONE_NODE_NAMES = "BoneNodeNames";
static std::string TOKEN_NODE_OFFSETS = "NodeOffsets";

ModelAnimatorDto::ModelAnimatorDto() : m_factoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName())
{
}

ModelAnimatorDto::ModelAnimatorDto(const GenericDto& dto) : m_factoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName())
{
    factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) id() = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_CONTROLLED_PRIMITIVE_ID)) controlledPrimitiveId() = v.value();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ASSET_ID)) animationAssetId() = v.value();
    if (auto v = dto.tryGetValue<GenericDtoCollection>(TOKEN_SKIN_OPERATORS)) skinOperators() = v.value();
}

GenericDto ModelAnimatorDto::toGenericDto()
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_ID, id().tokens());
    dto.addRtti(m_factoryDesc);
    if (m_controlledPrimitiveId) dto.addOrUpdate(TOKEN_CONTROLLED_PRIMITIVE_ID, m_controlledPrimitiveId.value().tokens());
    if (m_animationAssetId) dto.addOrUpdate(TOKEN_ASSET_ID, m_animationAssetId.value().name());
    dto.addOrUpdate(TOKEN_SKIN_OPERATORS, m_skinOperators);
    return dto;
}

/*std::shared_ptr<ModelAnimatorPolicy> ModelAnimatorDto::convertToPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled,
    const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    std::shared_ptr<AnimationAssetPolicy> asset_policy;
    std::shared_ptr<ModelAnimatorPolicy> policy;
    if (m_animationAssetDto)
    {
        asset_policy = std::make_shared<AnimationAssetPolicy>(m_assetName, m_animationAssetDto.value());
        policy = std::make_shared<ModelAnimatorPolicy>(controlled, asset_policy);
        policy->AssetFactoryDesc() = m_animationAssetDto.value().getRtti();
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
}*/

SkinOperatorDto::SkinOperatorDto() : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
}

SkinOperatorDto::SkinOperatorDto(const Engine::GenericDto& dto) : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
    factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_SKIN_MESH_ID)) skinMeshId() = v.value();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_SKIN_MESH_NODE_NAME)) skinMeshNodeName() = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_BONE_NODE_NAMES)) boneNodeNames() = v.value();
    if (auto v = dto.tryGetValue<std::vector<MathLib::Matrix4>>(TOKEN_NODE_OFFSETS)) nodeOffsets() = v.value();
}

GenericDto SkinOperatorDto::toGenericDto()
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    if (m_skinMeshId) dto.addOrUpdate(TOKEN_SKIN_MESH_ID, m_skinMeshId.value().tokens());
    if (m_skinMeshNodeName) dto.addOrUpdate(TOKEN_SKIN_MESH_NODE_NAME, m_skinMeshNodeName.value());
    dto.addOrUpdate(TOKEN_BONE_NODE_NAMES, m_boneNodeNames);
    if (m_nodeOffsets) dto.addOrUpdate(TOKEN_NODE_OFFSETS, m_nodeOffsets);
    return dto;
}
