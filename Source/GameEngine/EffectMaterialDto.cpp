#include "EffectMaterialDto.h"
#include "EffectMaterial.h"
#include "EffectMaterialPolicy.h"
#include <memory>

using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";

EffectMaterialDto::EffectMaterialDto() : m_factoryDesc(EffectMaterial::TYPE_RTTI.getName())
{
}

EffectMaterialDto EffectMaterialDto::FromGenericDto(const GenericDto& dto)
{
    EffectMaterialDto effect;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) effect.Name() = v.value();
    effect.TheFactoryDesc() = dto.GetRtti();
    return effect;
}

GenericDto EffectMaterialDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    return dto;
}

EffectMaterialPolicy EffectMaterialDto::ConvertToPolicy(const std::shared_ptr<IEffectCompilingProfileDeserializer>& deserializer) const
{
    return EffectMaterialPolicy(m_name, m_factoryDesc.GetResourceFilename(), deserializer);
}
