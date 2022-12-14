#include "EffectTextureMap.h"
#include "EngineErrors.h"
#include <cassert>

using namespace Enigma::Engine;

EffectTextureMap::EffectTextureMap()
{

}

EffectTextureMap::EffectTextureMap(const EffectTextureTuple& tuple)
{
    m_effectTextures.emplace_back(tuple);
}

EffectTextureMap::EffectTextureMap(const EffectTextures& textures)
{
    m_effectTextures = textures;
}

EffectTextureMap::~EffectTextureMap()
{
    m_effectTextures.clear();
}

error EffectTextureMap::BindTexture(const EffectTextureTuple& tuple)
{
    auto index = GetTextureIndexBySemantic(std::get<std::string>(tuple));
    if (!index) // semantic not match
    {
        m_effectTextures.emplace_back(tuple);
        return ErrorCode::ok;
    }
    m_effectTextures[index.value()] = tuple;
    return ErrorCode::ok;
}

error EffectTextureMap::ChangeTexture(const EffectTextureTuple& tuple)
{
    auto index = GetTextureIndexBySemantic(std::get<std::string>(tuple));
    if (!index) return ErrorCode::textureSemantic;
    m_effectTextures[index.value()] = tuple;
    return ErrorCode::ok;
}

unsigned EffectTextureMap::AppendTextureSemantic(const std::string& semantic)
{
    auto index = GetTextureIndexBySemantic(semantic);
    if (index) return index.value();

    m_effectTextures.emplace_back(std::make_tuple(semantic, nullptr, std::nullopt));
    return static_cast<unsigned>(m_effectTextures.size()) - 1;
}

std::shared_ptr<Texture> EffectTextureMap::GetTexture(unsigned index)
{
    if (index >= m_effectTextures.size()) return nullptr;
    return std::get<std::shared_ptr<Texture>>(m_effectTextures[index]);
}

const EffectTextureMap::EffectTextureTuple& EffectTextureMap::GetEffectTextureTuple(unsigned index)
{
    assert(index < m_effectTextures.size());
    return m_effectTextures[index];
}

void EffectTextureMap::MergeTextureSetTo(EffectTextureMap& targetMap)
{
    for (auto& tex : m_effectTextures)
    {
        // skip null semantic, null texture
        if (std::get<std::shared_ptr<Texture>>(tex) == nullptr) continue;
        if (std::get<std::string>(tex).empty()) continue;

        targetMap.BindTexture(tex);
    }
}

std::optional<unsigned> EffectTextureMap::GetTextureIndexBySemantic(const std::string& semantic)
{
    if (m_effectTextures.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_effectTextures.size(); i++)
    {
        if (std::get<std::string>(m_effectTextures[i]) == semantic) return i;
    }
    return std::nullopt;
}
