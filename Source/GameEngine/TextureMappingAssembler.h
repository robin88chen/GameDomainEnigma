/*********************************************************************
 * \file   TextureMappingAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef TEXTURE_MAPPING_ASSEMBLER_H
#define TEXTURE_MAPPING_ASSEMBLER_H

#include "TextureId.h"
#include "GameEngine/GenericDto.h"
#include "EffectTextureMap.h"
#include <optional>
#include <string>

namespace Enigma::Engine
{
    class TextureMappingAssembler
    {
    public:
        TextureMappingAssembler() = default;
        TextureMappingAssembler(const EffectTextureMap::SemanticTextureMapping& mapping);

        void textureId(const TextureId& id) { m_mapping.m_textureId = id; }
        void arrayIndex(unsigned index) { m_mapping.m_arrayIndex = index; }
        void semantic(const std::string& semantic) { m_mapping.m_semantic = semantic; }

        Engine::GenericDto assemble() const;

    protected:
        EffectTextureMap::SemanticTextureMapping m_mapping;
    };

    class TextureMappingDisassembler
    {
    public:
        TextureMappingDisassembler() = default;
        TextureMappingDisassembler(const Engine::GenericDto& dto);

        [[nodiscard]] const EffectTextureMap::SemanticTextureMapping& semanticTextureMapping() const { return m_mapping; }
        [[nodiscard]] const TextureId& textureId() const { return m_mapping.m_textureId; }
        [[nodiscard]] std::optional<unsigned> arrayIndex() const { return m_mapping.m_arrayIndex; }
        [[nodiscard]] const std::string& semantic() const { return m_mapping.m_semantic; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        EffectTextureMap::SemanticTextureMapping m_mapping;
    };
}

#endif // TEXTURE_MAPPING_ASSEMBLER_H
