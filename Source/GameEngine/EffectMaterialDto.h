/*********************************************************************
 * \file   EffectMaterialDto.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_MATERIAL_DTO_H
#define _EFFECT_MATERIAL_DTO_H

#include "FactoryDesc.h"
#include "GenericDto.h"
#include <string>
#include <memory>

namespace Enigma::Engine
{
    class EffectMaterialPolicy;
    class IEffectCompilingProfileDeserializer;

    class EffectMaterialDto
    {
    public:
        EffectMaterialDto();
        EffectMaterialDto(const EffectMaterialDto&) = default;
        EffectMaterialDto(EffectMaterialDto&&) = default;
        ~EffectMaterialDto() = default;
        EffectMaterialDto& operator=(const EffectMaterialDto&) = default;
        EffectMaterialDto& operator=(EffectMaterialDto&&) = default;

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        FactoryDesc& factoryDesc() { return m_factoryDesc; }

        static EffectMaterialDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;

        EffectMaterialPolicy ConvertToPolicy(const std::shared_ptr<IEffectCompilingProfileDeserializer>& deserializer) const;

    protected:
        std::string m_name;
        FactoryDesc m_factoryDesc;
    };
}

#endif // _EFFECT_MATERIAL_DTO_H
