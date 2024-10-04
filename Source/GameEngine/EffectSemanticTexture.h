/*********************************************************************
 * \file   EffectSemanticTexture.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2024
 *********************************************************************/
#ifndef EFFECT_SEMANTIC_TEXTURE_H
#define EFFECT_SEMANTIC_TEXTURE_H

#include "TextureId.h"
#include "Texture.h"
#include <string>
#include <optional>

namespace Enigma::Engine
{
    class EffectSemanticTexture
    {
    public:
        EffectSemanticTexture();
        EffectSemanticTexture(const TextureId& texture_id, const std::optional<unsigned>& arrayIndex, const std::string& semantic);
        EffectSemanticTexture(const EffectSemanticTexture&) = default;
        EffectSemanticTexture(EffectSemanticTexture&&) = default;
        virtual ~EffectSemanticTexture();
        EffectSemanticTexture& operator=(const EffectSemanticTexture&) = default;
        EffectSemanticTexture& operator=(EffectSemanticTexture&&) = default;

        const TextureId& textureId() const { return m_textureId; }
        std::optional<unsigned> arrayIndex() const { return m_arrayIndex; }
        const std::string& semantic() const { return m_semantic; }
        const std::shared_ptr<Texture>& texture() const { return m_texture; }

    protected:
        TextureId m_textureId;
        std::optional<unsigned> m_arrayIndex;
        std::string m_semantic;
        std::shared_ptr<Texture> m_texture;
    };
}

#endif // EFFECT_SEMANTIC_TEXTURE_H
