#include "EffectDtoHelper.h"
#include "EffectMaterial.h"

using namespace Enigma::Engine;

EffectTextureMapDtoHelper::EffectTextureMapDtoHelper()
{
}

EffectTextureMapDtoHelper& EffectTextureMapDtoHelper::textureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic)
{
    TextureMappingDto tex;
    tex.textureId() = id;
    tex.semantic() = semantic;
    if (array_index) tex.arrayIndex() = array_index.value();
    m_dto.textureMappings().emplace_back(tex);
    return *this;
}

GenericDto EffectTextureMapDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}
