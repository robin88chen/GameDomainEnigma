#include "EffectMaterialDto.h"
#include "EffectMaterial.h"
#include "EffectMaterialPolicy.h"
#include "EffectMaterialId.h"
#include <memory>

using namespace Enigma::Engine;

static std::string TOKEN_ID = "ID";

EffectMaterialDto::EffectMaterialDto() : m_factoryDesc(EffectMaterial::TYPE_RTTI.getName())
{
}

EffectMaterialDto EffectMaterialDto::fromGenericDto(const GenericDto& dto)
{
    EffectMaterialDto effect;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_ID)) effect.id() = EffectMaterialId(v.value());
    effect.factoryDesc() = dto.GetRtti();
    return effect;
}

GenericDto EffectMaterialDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_ID, m_id.name());
    return dto;
}

EffectMaterialPolicy EffectMaterialDto::ConvertToPolicy(const std::shared_ptr<IEffectCompilingProfileDeserializer>& deserializer) const
{
    return EffectMaterialPolicy(m_id.name(), m_factoryDesc.GetResourceFilename(), deserializer);
}
