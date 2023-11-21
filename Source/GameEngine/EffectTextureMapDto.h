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

        [[nodiscard]] TexturePolicy::JobType JobType() const { return m_jobType; }
        TexturePolicy::JobType& JobType() { return m_jobType; }
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
        [[nodiscard]] const MathLib::Dimension<unsigned>& Dimension() const { return m_dimension; }
        MathLib::Dimension<unsigned>& Dimension() { return m_dimension; }
        [[nodiscard]] unsigned SurfaceCount() const { return m_scurfaceCount; }
        unsigned& SurfaceCount() { return m_scurfaceCount; }

        static TextureMappingDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;

        EffectTextureMapPolicy::TextureTuplePolicy ConvertToPolicy();

    protected:
        TexturePolicy::JobType m_jobType;
        std::string m_textureName;
        std::string m_filename;
        std::string m_pathId;
        std::string m_semantic;
        std::optional<unsigned> m_arrayIndex;
        MathLib::Dimension<unsigned> m_dimension;
        unsigned m_scurfaceCount;
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

        static EffectTextureMapDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto();

        EffectTextureMapPolicy ConvertToPolicy();

    protected:
        std::vector<TextureMappingDto> m_textureMappings;
    };
}

#endif // _EFFECT_TEXTURE_MAP_DTO_H
