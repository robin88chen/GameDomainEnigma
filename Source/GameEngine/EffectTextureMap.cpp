#include "EffectTextureMap.h"
#include "EngineErrors.h"
#include "EffectTextureMapDto.h"
#include "Texture.h"
#include <tuple>
#include <string>
#include <memory>
#include <cassert>

using namespace Enigma::Engine;

EffectTextureMap::EffectTextureMap()
{

}

EffectTextureMap::EffectTextureMap(const GenericDto& dto)
{
    EffectTextureMapDto effectTextureMapDto = EffectTextureMapDto(dto);
    for (auto& mapping : effectTextureMapDto.textureMappings())
    {
        if (auto tex = Texture::queryTexture(mapping.textureId()); tex)
        {
            m_effectTextures.emplace_back(std::make_tuple(mapping.semantic(), tex, mapping.arrayIndex()));
        }
    }
}

EffectTextureMap::EffectTextureMap(const EffectSemanticTextureTuple& tuple)
{
    m_effectTextures.emplace_back(tuple);
}

EffectTextureMap::EffectTextureMap(const SegmentEffectTextures& textures)
{
    m_effectTextures = textures;
}

EffectTextureMap::~EffectTextureMap()
{
    m_effectTextures.clear();
}

GenericDto EffectTextureMap::serializeDto() const
{
    EffectTextureMapDto dto;
    for (auto& tex : m_effectTextures)
    {
        if (auto& t = std::get<std::shared_ptr<Texture>>(tex))
        {
            if (t->factoryDesc().GetResourceName().empty()) continue; // skip null texture (not resource texture)
            TextureMappingDto mapping;
            mapping.textureId(t->id());
            mapping.semantic(std::get<std::string>(tex));
            if (auto& v = std::get<std::optional<unsigned>>(tex)) mapping.arrayIndex(v.value());
            dto.addTextureMapping(mapping);
        }
    }
    return dto.toGenericDto();
}

error EffectTextureMap::bindSemanticTexture(const EffectSemanticTextureTuple& tuple)
{
    auto index = getTextureIndexBySemantic(std::get<std::string>(tuple));
    if (!index) // semantic not match
    {
        m_effectTextures.emplace_back(tuple);
        return ErrorCode::ok;
    }
    m_effectTextures[index.value()] = tuple;
    return ErrorCode::ok;
}

error EffectTextureMap::changeSemanticTexture(const EffectSemanticTextureTuple& tuple)
{
    auto index = getTextureIndexBySemantic(std::get<std::string>(tuple));
    if (!index) return ErrorCode::textureSemantic;
    m_effectTextures[index.value()] = tuple;
    return ErrorCode::ok;
}

unsigned EffectTextureMap::appendTextureSemantic(const std::string& semantic)
{
    auto index = getTextureIndexBySemantic(semantic);
    if (index) return index.value();

    m_effectTextures.emplace_back(std::make_tuple(semantic, nullptr, std::nullopt));
    return static_cast<unsigned>(m_effectTextures.size()) - 1;
}

std::shared_ptr<Texture> EffectTextureMap::getTexture(unsigned index)
{
    if (index >= m_effectTextures.size()) return nullptr;
    return std::get<std::shared_ptr<Texture>>(m_effectTextures[index]);
}

std::shared_ptr<Texture> EffectTextureMap::getTexture(unsigned index) const
{
    if (index >= m_effectTextures.size()) return nullptr;
    return std::get<std::shared_ptr<Texture>>(m_effectTextures[index]);
}

const EffectTextureMap::EffectSemanticTextureTuple& EffectTextureMap::getEffectSemanticTextureTuple(unsigned index)
{
    assert(index < m_effectTextures.size());
    return m_effectTextures[index];
}

std::optional<EffectTextureMap::EffectSemanticTextureTuple> EffectTextureMap::findSemanticTexture(const std::string& semantic) const
{
    for (auto& tuple : m_effectTextures)
    {
        if (std::get<std::string>(tuple) == semantic) return tuple;
    }
    return std::nullopt;
}

bool EffectTextureMap::isAllResourceTexture() const
{
    for (auto& tuple : m_effectTextures)
    {
        if (auto tex = std::get<std::shared_ptr<Texture>>(tuple); !tex)
        {
            if (tex->factoryDesc().GetResourceName().empty()) return false;
        }
    }
    return true;
}

void EffectTextureMap::mergeTextureSetTo(EffectTextureMap& targetMap)
{
    for (auto& tex : m_effectTextures)
    {
        // skip null semantic, null texture
        if (std::get<std::shared_ptr<Texture>>(tex) == nullptr) continue;
        if (std::get<std::string>(tex).empty()) continue;

        targetMap.bindSemanticTexture(tex);
    }
}

std::optional<unsigned> EffectTextureMap::getTextureIndexBySemantic(const std::string& semantic)
{
    if (m_effectTextures.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_effectTextures.size(); i++)
    {
        if (std::get<std::string>(m_effectTextures[i]) == semantic) return i;
    }
    return std::nullopt;
}
