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
#include <optional>
#include <string>

namespace Enigma::Engine
{
    class TextureMappingAssembler
    {
    public:
        TextureMappingAssembler() = default;
        TextureMappingAssembler(const TextureId& id, const std::optional<unsigned>& array_index, const std::string& semantic);

        void textureId(const TextureId& id) { m_textureId = id; }
        void arrayIndex(unsigned index) { m_arrayIndex = index; }
        void semantic(const std::string& semantic) { m_semantic = semantic; }

        Engine::GenericDto assemble() const;

    protected:
        TextureId m_textureId;
        std::optional<unsigned> m_arrayIndex;
        std::string m_semantic;
    };

    class TextureMappingDisassembler
    {
    public:
        TextureMappingDisassembler() = default;
        TextureMappingDisassembler(const Engine::GenericDto& dto);

        [[nodiscard]] const TextureId& textureId() const { return m_textureId; }
        [[nodiscard]] std::optional<unsigned> arrayIndex() const { return m_arrayIndex; }
        [[nodiscard]] const std::string& semantic() const { return m_semantic; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        TextureId m_textureId;
        std::optional<unsigned> m_arrayIndex;
        std::string m_semantic;
    };
}

#endif // TEXTURE_MAPPING_ASSEMBLER_H
