#include "AnimatorDtos.h"
#include "AnimationAssetPolicies.h"
#include "AnimatorPolicies.h"
#include "ModelPrimitiveAnimator.h"

using namespace Enigma::Animators;
using namespace Enigma::Engine;

static std::string TOKEN_ANIMATION_NAME = "AnimationName";
static std::string TOKEN_ASSET_OBJECT = "AssetObject";
static std::string TOKEN_ANIMATION_FACTORY = "AnimationFactory";

ModelAnimatorDto::ModelAnimatorDto() : m_assetFactory(ModelPrimitiveAnimator::TYPE_RTTI.GetName())
{
}

ModelAnimatorDto ModelAnimatorDto::FromGenericDto(const GenericDto& dto)
{
    ModelAnimatorDto model;
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_ASSET_OBJECT)) model.AnimationAssetDto() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_ANIMATION_NAME)) model.AssetName() = v.value();
    if (auto v = dto.TryGetValue<FactoryDesc>(TOKEN_ANIMATION_FACTORY)) model.AssetFactoryDesc() = v.value();
    return model;
}

GenericDto ModelAnimatorDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.GetName()));
    if (m_animationAssetDto) dto.AddOrUpdate(TOKEN_ASSET_OBJECT, m_animationAssetDto.value());
    dto.AddOrUpdate(TOKEN_ANIMATION_NAME, m_assetName);
    dto.AddOrUpdate(TOKEN_ANIMATION_FACTORY, m_assetFactory);
    return dto;
}

std::shared_ptr<ModelAnimatorPolicy> ModelAnimatorDto::ConvertToPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled,
    const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    std::shared_ptr<AnimationAssetPolicy> asset_policy;
    if (m_animationAssetDto)
    {
        asset_policy = std::make_shared<AnimationAssetPolicy>(m_assetName, m_animationAssetDto.value());
    }
    else
    {
        asset_policy = std::make_shared<AnimationAssetPolicy>(m_assetName, m_assetFactory.GetResourceFilename(), deserializer);
    }
    auto policy = std::make_shared<ModelAnimatorPolicy>(controlled, asset_policy);
    return policy;
}
