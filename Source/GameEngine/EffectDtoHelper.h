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
#include "TextureId.h"

namespace Enigma::Engine
{
    class EffectTextureMapDtoHelper
    {
    public:
        EffectTextureMapDtoHelper();

        EffectTextureMapDtoHelper& textureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic);

        GenericDto toGenericDto();

    protected:
        EffectTextureMapDto m_dto;
    };
}

#endif // EFFECT_DTO_HELPER_H
