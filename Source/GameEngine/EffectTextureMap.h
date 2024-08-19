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
    class EffectTextureMapAssembler;
    class EffectTextureMapDisassembler;

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

        void assemble(const std::shared_ptr<EffectTextureMapAssembler>& assembler) const;
        void disassemble(const std::shared_ptr<EffectTextureMapDisassembler>& disassembler);

        error bindSemanticTexture(const EffectSemanticTextureTuple& tuple);
        error changeSemanticTexture(const EffectSemanticTextureTuple& tuple);
        unsigned appendTextureSemantic(const std::string& semantic);  ///< return index
        std::shared_ptr<Texture> getTexture(unsigned index);
        std::shared_ptr<Texture> getTexture(unsigned index) const;
        const EffectSemanticTextureTuple& getEffectSemanticTextureTuple(unsigned index);
        const unsigned int getCount() { return static_cast<unsigned int>(m_effectTextures.size()); };
        std::optional<EffectSemanticTextureTuple> findSemanticTexture(const std::string& semantic) const;

        bool isAllResourceTexture() const;

        /// merge texture sets to target map, with respect to same semantic
        void mergeTextureSetTo(EffectTextureMap& targetMap);

    protected:
        std::optional<unsigned> getTextureIndexBySemantic(const std::string& semantic);

    protected:
        SegmentEffectTextures m_effectTextures;
    };
}

#endif // _EFFECT_TEXTURE_MAP_H
