/*********************************************************************
 * \file   EffectMaterialSourceStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_MATERIAL_SOURCE_STORE_MAPPER_H
#define EFFECT_MATERIAL_SOURCE_STORE_MAPPER_H

#include "EffectMaterialId.h"
#include "EffectCompilingProfile.h"
#include <system_error>

namespace Enigma::Engine
{
    class EffectMaterialSourceStoreMapper
    {
    public:
        virtual ~EffectMaterialSourceStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasEffectMaterial(const EffectMaterialId& id) = 0;
        virtual std::optional<EffectCompilingProfile> queryEffectMaterial(const EffectMaterialId& id) = 0;
    };
}

#endif // EFFECT_MATERIAL_SOURCE_STORE_MAPPER_H
