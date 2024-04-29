#include "EffectTextureMapAssembler.h"
#include "EffectMaterial.h"

using namespace Enigma::Engine;

EffectTextureMapAssembler::EffectTextureMapAssembler()
{
}

EffectTextureMapAssembler& EffectTextureMapAssembler::textureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic)
{
    TextureMappingDto tex;
    tex.textureId() = id;
    tex.semantic() = semantic;
    if (array_index) tex.arrayIndex() = array_index.value();
    m_dto.textureMappings().emplace_back(tex);
    return *this;
}

GenericDto EffectTextureMapAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}
