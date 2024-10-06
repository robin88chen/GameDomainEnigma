/*********************************************************************
 * \file   EffectSemanticTextureAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef EFFECT_SEMANTIC_TEXTURE_ASSEMBLER_H
#define EFFECT_SEMANTIC_TEXTURE_ASSEMBLER_H

#include "TextureId.h"
#include "GameEngine/GenericDto.h"
#include "EffectSemanticTexture.h"
#include <optional>
#include <string>

namespace Enigma::Engine
{
    class EffectSemanticTextureAssembler
    {
    public:
        EffectSemanticTextureAssembler() = default;
        EffectSemanticTextureAssembler(const EffectSemanticTexture& semantic_texture);

        void semanticTexture(const EffectSemanticTexture& semantic_texture) { m_semanticTexture = semantic_texture; }

        Engine::GenericDto assemble() const;

    protected:
        EffectSemanticTexture m_semanticTexture;
    };

    class EffectSemanticTextureDisassembler
    {
    public:
        EffectSemanticTextureDisassembler() = default;
        EffectSemanticTextureDisassembler(const Engine::GenericDto& dto);

        [[nodiscard]] const EffectSemanticTexture& semanticTexture() const { return m_semanticTexture; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        EffectSemanticTexture m_semanticTexture;
    };
}

#endif // EFFECT_SEMANTIC_TEXTURE_ASSEMBLER_H
