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
        TextureMappingDto(const GenericDto& dto);
        TextureMappingDto(const TextureMappingDto&) = default;
        TextureMappingDto(TextureMappingDto&&) = default;
        ~TextureMappingDto() = default;
        TextureMappingDto& operator=(const TextureMappingDto&) = default;
        TextureMappingDto& operator=(TextureMappingDto&&) = default;

        [[nodiscard]] const TextureId& textureId() const { return m_textureId; }
        void textureId(const TextureId& id) { m_textureId = id; }
        [[nodiscard]] const std::string& semantic() const { return m_semantic; }
        void semantic(const std::string& semantic) { m_semantic = semantic; }
        [[nodiscard]] std::optional<unsigned> arrayIndex() const { return m_arrayIndex; }
        void arrayIndex(unsigned index) { m_arrayIndex = index; }

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
        EffectTextureMapDto(const GenericDto& dto);
        EffectTextureMapDto(const EffectTextureMapDto&) = default;
        EffectTextureMapDto(EffectTextureMapDto&&) = default;
        ~EffectTextureMapDto() = default;
        EffectTextureMapDto& operator=(const EffectTextureMapDto&) = default;
        EffectTextureMapDto& operator=(EffectTextureMapDto&&) = default;

        [[nodiscard]] const std::vector<TextureMappingDto>& textureMappings() const { return m_textureMappings; }
        void textureMappings(const std::vector<TextureMappingDto>& mapping_dtos) { m_textureMappings = mapping_dtos; }
        void addTextureMapping(const TextureMappingDto& mapping_dto) { m_textureMappings.emplace_back(mapping_dto); }

        GenericDto toGenericDto();

    protected:
        std::vector<TextureMappingDto> m_textureMappings;
    };
}

#endif // _EFFECT_TEXTURE_MAP_DTO_H
