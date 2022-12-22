/*********************************************************************
 * \file   EffectTextureMap.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_TEXTURE_MAP_H
#define _EFFECT_TEXTURE_MAP_H

#include <system_error>
#include <memory>
#include <optional>
#include <vector>

namespace Enigma::Engine
{
    using error = std::error_code;

    class Texture;
    class GenericDto;

    class EffectTextureMap
    {
    public:
        using EffectTextureTuple = std::tuple<std::string, std::shared_ptr<Texture>, std::optional<unsigned>>;
        using EffectTextures = std::vector<EffectTextureTuple>;
    public:
        EffectTextureMap();
        EffectTextureMap(const EffectTextureTuple& tuple);
        EffectTextureMap(const EffectTextures& textures);
        EffectTextureMap(const EffectTextureMap&) = default;
        EffectTextureMap(EffectTextureMap&&) = default;
        virtual ~EffectTextureMap();
        EffectTextureMap& operator=(const EffectTextureMap&) = default;
        EffectTextureMap& operator=(EffectTextureMap&&) = default;

        GenericDto SerializeDto();

        error BindTexture(const EffectTextureTuple& tuple);
        error ChangeTexture(const EffectTextureTuple& tuple);
        unsigned AppendTextureSemantic(const std::string& semantic);  ///< return index
        std::shared_ptr<Texture> GetTexture(unsigned index);
        const EffectTextureTuple& GetEffectTextureTuple(unsigned index);
        const unsigned int GetCount() { return static_cast<unsigned int>(m_effectTextures.size()); };

        /// merge texture sets to target map, with respect to same semantic
        void MergeTextureSetTo(EffectTextureMap& targetMap);

    protected:
        std::optional<unsigned> GetTextureIndexBySemantic(const std::string& semantic);

    protected:
        EffectTextures m_effectTextures;
    };
}

#endif // _EFFECT_TEXTURE_MAP_H
