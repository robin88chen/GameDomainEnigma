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
#include "EffectTextureMapPolicy.h"
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

        [[nodiscard]] const std::string& TextureName() const { return m_textureName; }
        std::string& TextureName() { return m_textureName; }
        [[nodiscard]] const std::string& Filename() const { return m_filename; }
        std::string& Filename() { return m_filename; }
        [[nodiscard]] const std::string& PathId() const { return m_pathId; }
        std::string& PathId() { return m_pathId; }
        [[nodiscard]] const std::string& Semantic() const { return m_semantic; }
        std::string& Semantic() { return m_semantic; }
        [[nodiscard]] std::optional<unsigned> ArrayIndex() const { return m_arrayIndex; }
        std::optional<unsigned>& ArrayIndex() { return m_arrayIndex; }

        static TextureMappingDto FromGenericDto(const GenericDto& dto);
        GenericDto ToGenericDto() const;

        EffectTextureMapPolicy::TextureTuplePolicy ConvertToPolicy();

    protected:
        std::string m_textureName;
        std::string m_filename;
        std::string m_pathId;
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

        [[nodiscard]] const std::vector<TextureMappingDto>& TextureMappings() const { return m_textureMappings; }
        std::vector<TextureMappingDto>& TextureMappings() { return m_textureMappings; }

        static EffectTextureMapDto FromGenericDto(const GenericDto& dto);
        GenericDto ToGenericDto();

        EffectTextureMapPolicy ConvertToPolicy();

    protected:
        std::vector<TextureMappingDto> m_textureMappings;
    };
}

#endif // _EFFECT_TEXTURE_MAP_DTO_H
