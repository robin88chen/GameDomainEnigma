#include "EffectTextureMapAssembler.h"
#include "EffectMaterial.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_MAPPINGS = "TextureMappings";

EffectTextureMapAssembler::EffectTextureMapAssembler()
{
}

void EffectTextureMapAssembler::addTextureMapping(const TextureId& id, std::optional<unsigned int> array_index, const std::string& semantic)
{
    TextureMappingAssembler assembler;
    assembler.textureId(id);
    if (array_index) assembler.arrayIndex(array_index.value());
    assembler.semantic(semantic);
    m_mappingAssemblers.emplace_back(assembler);
}

GenericDto EffectTextureMapAssembler::assemble() const
{
    GenericDtoCollection mappings;
    for (auto& tex : m_mappingAssemblers)
    {
        mappings.emplace_back(tex.assemble());
    }
    GenericDto dto;
    dto.addOrUpdate(TOKEN_TEXTURE_MAPPINGS, mappings);
    return dto;
}

EffectTextureMapDisassembler::EffectTextureMapDisassembler()
{
}

void EffectTextureMapDisassembler::disassemble(const GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<GenericDtoCollection>(TOKEN_TEXTURE_MAPPINGS))
    {
        for (auto& mapping_dto : v.value())
        {
            TextureMappingDisassembler disassembler;
            disassembler.disassemble(mapping_dto);
            m_mappingDisassemblers.emplace_back(disassembler);
        }
    }
}
