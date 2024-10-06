/*********************************************************************
 * \file   EffectDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef EFFECT_TEXTURE_MAP_ASSEMBLER_H
#define EFFECT_TEXTURE_MAP_ASSEMBLER_H

#include "TextureId.h"
#include "GenericDto.h"
#include "EffectSemanticTextureAssembler.h"
#include <optional>
#include <string>
#include <vector>

namespace Enigma::Engine
{
    class EffectTextureMapAssembler
    {
    public:
        EffectTextureMapAssembler();

        void addSemanticTextureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic);
        void addSemanticTextureMapping(const EffectSemanticTexture& semantic_texture);

        GenericDto assemble() const;

    protected:
        std::vector<EffectSemanticTextureAssembler> m_mappingAssemblers;
    };

    class EffectTextureMapDisassembler
    {
    public:
        EffectTextureMapDisassembler();
        EffectTextureMapDisassembler(const GenericDto& dto);

        [[nodiscard]] const std::vector<EffectSemanticTextureDisassembler>& semanticTextureMappings() const { return m_mappingDisassemblers; }

        void disassemble(const GenericDto& dto);

    protected:
        std::vector<EffectSemanticTextureDisassembler> m_mappingDisassemblers;
    };
}

#endif // EFFECT_TEXTURE_MAP_ASSEMBLER_H
