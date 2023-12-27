/*********************************************************************
 * \file   EffectTextureMapDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_TEXTURE_MAP_DTO_H
#define _EFFECT_TEXTURE_MAP_DTO_H

#include "GenericDto.h"
#include "TextureId.h"
#include <string>
#include <optional>
#include <vector>


namespace Enigma::Engine
{
    class TextureMappingDto
    {
    public:
        TextureMappingDto() = default;
        TextureMappingDto(const TextureMappingDto&) = default;
        TextureMappingDto(TextureMappingDto&&) = default;
        ~TextureMappingDto() = default;
        TextureMappingDto& operator=(const TextureMappingDto&) = default;
        TextureMappingDto& operator=(TextureMappingDto&&) = default;

        [[nodiscard]] const TextureId& textureId() const { return m_textureId; }
        TextureId& textureId() { return m_textureId; }
        [[nodiscard]] const std::string& semantic() const { return m_semantic; }
        std::string& semantic() { return m_semantic; }
        [[nodiscard]] std::optional<unsigned> arrayIndex() const { return m_arrayIndex; }
        std::optional<unsigned>& arrayIndex() { return m_arrayIndex; }

        static TextureMappingDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;

    protected:
        TextureId m_textureId;
        std::string m_semantic;
        std::optional<unsigned> m_arrayIndex;
    };

    class EffectTextureMapDto
    {
    public:
        EffectTextureMapDto() = default;
        EffectTextureMapDto(const EffectTextureMapDto&) = default;
        EffectTextureMapDto(EffectTextureMapDto&&) = default;
        ~EffectTextureMapDto() = default;
        EffectTextureMapDto& operator=(const EffectTextureMapDto&) = default;
        EffectTextureMapDto& operator=(EffectTextureMapDto&&) = default;

        [[nodiscard]] const std::vector<TextureMappingDto>& textureMappings() const { return m_textureMappings; }
        std::vector<TextureMappingDto>& textureMappings() { return m_textureMappings; }

        static EffectTextureMapDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto();

    protected:
        std::vector<TextureMappingDto> m_textureMappings;
    };
}

#endif // _EFFECT_TEXTURE_MAP_DTO_H
