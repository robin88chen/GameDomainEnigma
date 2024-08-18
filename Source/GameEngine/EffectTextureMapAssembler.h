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
#include "TextureMappingAssembler.h"
#include <optional>
#include <string>
#include <vector>

namespace Enigma::Engine
{
    class EffectTextureMapAssembler
    {
    public:
        EffectTextureMapAssembler();

        void addTextureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic);

        GenericDto assemble() const;

    protected:
        std::vector<TextureMappingAssembler> m_mappingAssemblers;
    };

    class EffectTextureMapDisassembler
    {
    public:
        EffectTextureMapDisassembler();

        [[nodiscard]] const std::vector<TextureMappingDisassembler>& textureMappings() const { return m_mappingDisassemblers; }

        void disassemble(const GenericDto& dto);

    protected:
        std::vector<TextureMappingDisassembler> m_mappingDisassemblers;
    };
}

#endif // EFFECT_TEXTURE_MAP_ASSEMBLER_H
