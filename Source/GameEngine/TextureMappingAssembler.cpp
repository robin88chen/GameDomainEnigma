#include "TextureMappingAssembler.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_ID_NAME = "TextureId.Name";
static std::string TOKEN_SEMANTIC = "Semantic";
static std::string TOKEN_ARRAY_INDEX = "ArrayIndex";

TextureMappingAssembler::TextureMappingAssembler(const TextureId& id, const std::optional<unsigned>& array_index, const std::string& semantic)
    : m_textureId(id), m_arrayIndex(array_index), m_semantic(semantic)
{
}

GenericDto TextureMappingAssembler::assemble() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_TEXTURE_ID_NAME, m_textureId.name());
    if (m_arrayIndex)
    {
        dto.addOrUpdate(TOKEN_ARRAY_INDEX, m_arrayIndex.value());
    }
    dto.addOrUpdate(TOKEN_SEMANTIC, m_semantic);
    return dto;
}

TextureMappingDisassembler::TextureMappingDisassembler(const Engine::GenericDto& dto)
{
    disassemble(dto);
}

void TextureMappingDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_TEXTURE_ID_NAME)) m_textureId = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_ARRAY_INDEX)) m_arrayIndex = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_SEMANTIC)) m_semantic = v.value();
}

