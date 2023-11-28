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
        using EffectSemanticTextureTuple = std::tuple<std::string, std::shared_ptr<Texture>, std::optional<unsigned>>;
        using SegmentEffectTextures = std::vector<EffectSemanticTextureTuple>;
    public:
        EffectTextureMap();
        EffectTextureMap(const EffectSemanticTextureTuple& tuple);
        EffectTextureMap(const SegmentEffectTextures& textures);
        EffectTextureMap(const EffectTextureMap&) = default;
        EffectTextureMap(EffectTextureMap&&) = default;
        virtual ~EffectTextureMap();
        EffectTextureMap& operator=(const EffectTextureMap&) = default;
        EffectTextureMap& operator=(EffectTextureMap&&) = default;

        GenericDto serializeDto() const;

        error BindSemanticTexture(const EffectSemanticTextureTuple& tuple);
        error ChangeSemanticTexture(const EffectSemanticTextureTuple& tuple);
        unsigned AppendTextureSemantic(const std::string& semantic);  ///< return index
        std::shared_ptr<Texture> GetTexture(unsigned index);
        std::shared_ptr<Texture> GetTexture(unsigned index) const;
        const EffectSemanticTextureTuple& GetEffectSemanticTextureTuple(unsigned index);
        const unsigned int GetCount() { return static_cast<unsigned int>(m_effectTextures.size()); };
        std::optional<EffectSemanticTextureTuple> FindSemanticTexture(const std::string& semantic) const;

        bool IsAllResourceTexture() const;

        /// merge texture sets to target map, with respect to same semantic
        void MergeTextureSetTo(EffectTextureMap& targetMap);

    protected:
        std::optional<unsigned> GetTextureIndexBySemantic(const std::string& semantic);

    protected:
        SegmentEffectTextures m_effectTextures;
    };
}

#endif // _EFFECT_TEXTURE_MAP_H
