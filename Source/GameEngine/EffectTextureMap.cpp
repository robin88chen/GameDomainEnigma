#include "EffectTextureMap.h"
#include "EngineErrors.h"
#include "EffectTextureMapAssembler.h"
#include "EffectSemanticTexture.h"
#include "Texture.h"
#include <tuple>
#include <string>
#include <memory>
#include <cassert>

using namespace Enigma::Engine;

EffectTextureMap::EffectTextureMap()
{
}

EffectTextureMap::EffectTextureMap(const EffectSemanticTexture& effect_semantic_texture)
{
    m_effectTextures.emplace_back(effect_semantic_texture);
}

EffectTextureMap::EffectTextureMap(const std::vector<EffectSemanticTexture>& mappings)
{
    m_effectTextures = mappings;
}

EffectTextureMap::~EffectTextureMap()
{
    m_effectTextures.clear();
}

void EffectTextureMap::assemble(const std::shared_ptr<EffectTextureMapAssembler>& assembler) const
{
    assert(assembler);
    for (const auto& tex : m_effectTextures)
    {
        assembler->addSemanticTextureMapping(tex.textureId(), tex.arrayIndex(), tex.semantic());
    }
}

void EffectTextureMap::disassemble(const std::shared_ptr<EffectTextureMapDisassembler>& disassembler)
{
    assert(disassembler);
    for (auto& mapping : disassembler->semanticTextureMappings())
    {
        m_effectTextures.emplace_back(mapping.semanticTexture());
    }
}

error EffectTextureMap::bindSemanticTexture(const EffectSemanticTexture& semantic_texture)
{
    auto index = getTextureIndexBySemantic(semantic_texture.semantic());
    if (!index) // semantic not match
    {
        m_effectTextures.emplace_back(semantic_texture);
        return ErrorCode::ok;
    }
    m_effectTextures[index.value()] = semantic_texture;
    return ErrorCode::ok;
}

error EffectTextureMap::changeSemanticTexture(const EffectSemanticTexture& semantic_texture)
{
    auto index = getTextureIndexBySemantic(semantic_texture.semantic());
    if (!index) return ErrorCode::textureSemantic;
    m_effectTextures[index.value()] = semantic_texture;
    return ErrorCode::ok;
}

const EffectSemanticTexture& EffectTextureMap::getEffectSemanticTexture(unsigned index) const
{
    assert(index < m_effectTextures.size());
    return m_effectTextures[index];
}

std::optional<EffectSemanticTexture> EffectTextureMap::findSemanticTexture(const std::string& semantic) const
{
    for (auto& tex : m_effectTextures)
    {
        if (tex.semantic() == semantic) return tex;
    }
    return std::nullopt;
}

bool EffectTextureMap::isAllTextureReady() const
{
    for (auto& tex : m_effectTextures)
    {
        if (!tex.texture()) return false;
        if (!tex.texture()->lazyStatus().isReady()) return false;
    }
    return true;
}

bool EffectTextureMap::hasTexture(const TextureId& textureId) const
{
    for (auto& tex : m_effectTextures)
    {
        if (tex.textureId() == textureId) return true;
    }
    return false;
}

std::optional<unsigned> EffectTextureMap::getTextureIndexBySemantic(const std::string& semantic)
{
    if (m_effectTextures.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_effectTextures.size(); i++)
    {
        if (m_effectTextures[i].semantic() == semantic) return i;
    }
    return std::nullopt;
}
