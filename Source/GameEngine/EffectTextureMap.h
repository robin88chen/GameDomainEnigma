/*********************************************************************
 * \file   EffectTextureMap.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_TEXTURE_MAP_H
#define _EFFECT_TEXTURE_MAP_H

#include "TextureId.h"
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
    class EffectSemanticTexture;

    class EffectTextureMap
    {
    public:
        EffectTextureMap();
        EffectTextureMap(const EffectSemanticTexture& effect_semantic_texture);
        EffectTextureMap(const std::vector<EffectSemanticTexture>& mappings);
        EffectTextureMap(const EffectTextureMap&) = default;
        EffectTextureMap(EffectTextureMap&&) = default;
        virtual ~EffectTextureMap();
        EffectTextureMap& operator=(const EffectTextureMap&) = default;
        EffectTextureMap& operator=(EffectTextureMap&&) = default;

        void assemble(const std::shared_ptr<EffectTextureMapAssembler>& assembler) const;
        void disassemble(const std::shared_ptr<EffectTextureMapDisassembler>& disassembler);

        error bindSemanticTexture(const EffectSemanticTexture& semantic_texture);
        error changeSemanticTexture(const EffectSemanticTexture& semantic_texture);
        const EffectSemanticTexture& getEffectSemanticTexture(unsigned index) const;
        unsigned int getCount() const { return static_cast<unsigned int>(m_effectTextures.size()); };
        std::optional<EffectSemanticTexture> findSemanticTexture(const std::string& semantic) const;

        bool isAllTextureReady() const;
        bool hasTexture(const TextureId& textureId) const;

    protected:
        std::optional<unsigned> getTextureIndexBySemantic(const std::string& semantic);

    protected:
        std::vector<EffectSemanticTexture> m_effectTextures;
    };
}

#endif // _EFFECT_TEXTURE_MAP_H
