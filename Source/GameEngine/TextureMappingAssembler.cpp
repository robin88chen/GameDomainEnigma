#include "TextureMappingAssembler.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_ID_NAME = "TextureId.Name";
static std::string TOKEN_SEMANTIC = "Semantic";
static std::string TOKEN_ARRAY_INDEX = "ArrayIndex";

GenericDto TextureMappingAssembler::assemble() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_TEXTURE_ID_NAME, m_textureId.name());
    dto.addOrUpdate(TOKEN_SEMANTIC, m_semantic);
    if (m_arrayIndex)
    {
        dto.addOrUpdate(TOKEN_ARRAY_INDEX, m_arrayIndex.value());
    }
    return dto;
}

void TextureMappingDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_TEXTURE_ID_NAME)) m_textureId = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_SEMANTIC)) m_semantic = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_ARRAY_INDEX)) m_arrayIndex = v.value();
}

