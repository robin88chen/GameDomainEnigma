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

        void textureId(const TextureId& id) { m_textureId = id; }
        void semantic(const std::string& semantic) { m_semantic = semantic; }
        void arrayIndex(unsigned index) { m_arrayIndex = index; }

        Engine::GenericDto assemble() const;

    protected:
        TextureId m_textureId;
        std::string m_semantic;
        std::optional<unsigned> m_arrayIndex;
    };

    class TextureMappingDisassembler
    {
    public:
        TextureMappingDisassembler() = default;

        [[nodiscard]] const TextureId& textureId() const { return m_textureId; }
        [[nodiscard]] const std::string& semantic() const { return m_semantic; }
        [[nodiscard]] std::optional<unsigned> arrayIndex() const { return m_arrayIndex; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        TextureId m_textureId;
        std::string m_semantic;
        std::optional<unsigned> m_arrayIndex;
    };
}

#endif // TEXTURE_MAPPING_ASSEMBLER_H
