/*********************************************************************
 * \file   EffectDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef EFFECT_DTO_HELPER_H
#define EFFECT_DTO_HELPER_H

#include "EffectTextureMapDto.h"

namespace Enigma::Engine
{
    /*class EffectMaterialDtoHelper
    {
    public:
        EffectMaterialDtoHelper(const std::string& name);

        EffectMaterialDtoHelper& FilenameAtPath(const std::string& filename, const std::string& path_id);
        EffectMaterialDtoHelper& FilenameAtPath(const std::string& filename_at_path);

        GenericDto toGenericDto();

    protected:
        EffectMaterialDto m_dto;
    };*/

    class EffectTextureMapDtoHelper
    {
    public:
        EffectTextureMapDtoHelper();

        EffectTextureMapDtoHelper& TextureMapping(const std::string& filename, const std::string& path_id,
            const std::string& tex_name, std::optional<unsigned> array_index, const std::string& semantic);
        EffectTextureMapDtoHelper& TextureMapping(const MathLib::Dimension<unsigned>& dimension, unsigned surface_count,
            const std::string& tex_name, std::optional<unsigned> array_index, const std::string& semantic);

        GenericDto toGenericDto();

    protected:
        EffectTextureMapDto m_dto;
    };
}

#endif // EFFECT_DTO_HELPER_H
