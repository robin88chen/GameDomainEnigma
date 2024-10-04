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

/*EffectTextureMap::EffectTextureMap(const SegmentEffectTextures& textures)
{
    m_effectTextures = textures;
}*/

EffectTextureMap::EffectTextureMap(const std::vector<EffectSemanticTexture>& mappings)
{
    m_effectTextures = mappings;
    /*for (auto& mapping : mappings)
    {
        m_effectTextures.emplace_back(std::make_tuple(Texture::queryTexture(mapping.m_textureId), mapping.m_arrayIndex, mapping.m_semantic));
    }*/
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
        /*if (auto& t = std::get<std::shared_ptr<Texture>>(tex))
        {
            //if (t->factoryDesc().GetResourceName().empty()) continue; // skip null texture (not resource texture)
            assembler->addTextureMapping(std::get<std::shared_ptr<Texture>>(tex)->id(), std::get<std::optional<unsigned>>(tex), std::get<std::string>(tex));
        }*/
    }
}

void EffectTextureMap::disassemble(const std::shared_ptr<EffectTextureMapDisassembler>& disassembler)
{
    assert(disassembler);
    for (auto& mapping : disassembler->semanticTextureMappings())
    {
        m_effectTextures.emplace_back(mapping.semanticTexture());
        /*if (auto tex = Texture::queryTexture(mapping.textureId()); tex)
        {
            m_effectTextures.emplace_back(std::make_tuple(tex, mapping.arrayIndex(), mapping.semantic()));
        }*/
    }
}

/*error EffectTextureMap::addSemanticTexture(const SemanticTextureMapping& mapping)
{
    if (mapping.m_semantic.empty()) return ErrorCode::textureSemantic;
    if (auto index = getTextureIndexBySemantic(mapping.m_semantic); index) return ErrorCode::textureSemantic;
    EffectSemanticTextureTuple tuple = std::make_tuple(Texture::queryTexture(mapping.m_textureId), mapping.m_arrayIndex, mapping.m_semantic);
    m_effectTextures.emplace_back(tuple);
    return ErrorCode::ok;
}*/

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

/*unsigned EffectTextureMap::appendTextureSemantic(const std::string& semantic)
{
    auto index = getTextureIndexBySemantic(semantic);
    if (index) return index.value();

    m_effectTextures.emplace_back(std::make_tuple(nullptr, std::nullopt, semantic));
    return static_cast<unsigned>(m_effectTextures.size()) - 1;
}*/

/*std::shared_ptr<Texture> EffectTextureMap::getTexture(unsigned index)
{
    if (index >= m_effectTextures.size()) return nullptr;
    return std::get<std::shared_ptr<Texture>>(m_effectTextures[index]);
}

std::shared_ptr<Texture> EffectTextureMap::getTexture(unsigned index) const
{
    if (index >= m_effectTextures.size()) return nullptr;
    return std::get<std::shared_ptr<Texture>>(m_effectTextures[index]);
}*/

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

/*std::shared_ptr<Texture> EffectTextureMap::findTexture(const TextureId& textureId) const
{
    for (auto& tex : m_effectTextures)
    {
        if (auto tex = std::get<std::shared_ptr<Texture>>(tuple); tex && tex->id() == textureId) return tex;
    }
    return nullptr;
}*/

/*bool EffectTextureMap::hasAnyResourceTexture() const
{
    for (auto& tex : m_effectTextures)
    {
        if (!tex) continue;
        //if (auto tex = std::get<std::shared_ptr<Texture>>(tuple); !tex)
        //{
        if (!tex->factoryDesc().GetResourceName().empty()) return true;
        //}
    }
    return false;
}*/

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

/*void EffectTextureMap::mergeTextureSetTo(EffectTextureMap& targetMap)
{
    for (auto& tex : m_effectTextures)
    {
        // skip null semantic, null texture
        if (std::get<std::shared_ptr<Texture>>(tex) == nullptr) continue;
        if (std::get<std::string>(tex).empty()) continue;

        targetMap.bindSemanticTexture(tex);
    }
}*/

std::optional<unsigned> EffectTextureMap::getTextureIndexBySemantic(const std::string& semantic)
{
    if (m_effectTextures.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_effectTextures.size(); i++)
    {
        if (m_effectTextures[i].semantic() == semantic) return i;
    }
    return std::nullopt;
}
