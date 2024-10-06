#include "EffectSemanticTextureAssembler.h"

using namespace Enigma::Engine;

static std::string TOKEN_TEXTURE_ID_NAME = "TextureId.Name";
static std::string TOKEN_SEMANTIC = "Semantic";
static std::string TOKEN_ARRAY_INDEX = "ArrayIndex";

EffectSemanticTextureAssembler::EffectSemanticTextureAssembler(const EffectSemanticTexture& semantic_texture)
    : m_semanticTexture(semantic_texture)
{
}

GenericDto EffectSemanticTextureAssembler::assemble() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_TEXTURE_ID_NAME, m_semanticTexture.textureId().name());
    if (m_semanticTexture.arrayIndex())
    {
        dto.addOrUpdate(TOKEN_ARRAY_INDEX, m_semanticTexture.arrayIndex().value());
    }
    dto.addOrUpdate(TOKEN_SEMANTIC, m_semanticTexture.semantic());
    return dto;
}

EffectSemanticTextureDisassembler::EffectSemanticTextureDisassembler(const Engine::GenericDto& dto)
{
    disassemble(dto);
}

void EffectSemanticTextureDisassembler::disassemble(const Engine::GenericDto& dto)
{
    TextureId textureId;
    std::optional<unsigned> array_index;
    std::string semantic;
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_TEXTURE_ID_NAME)) textureId = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_ARRAY_INDEX)) array_index = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_SEMANTIC)) semantic = v.value();
    m_semanticTexture = EffectSemanticTexture(textureId, array_index, semantic);
}

