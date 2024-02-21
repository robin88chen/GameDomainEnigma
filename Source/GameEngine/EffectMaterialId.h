/*********************************************************************
 * \file   EffectMaterialId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_MATERIAL_ID_H
#define EFFECT_MATERIAL_ID_H

#include <string>

namespace Enigma::Engine
{
    class EffectMaterialId
    {
    public:
        EffectMaterialId() = default;
        EffectMaterialId(const std::string& name);
        EffectMaterialId(const EffectMaterialId& source, std::uint32_t instance_serial);
        EffectMaterialId(const EffectMaterialId& other);
        EffectMaterialId(EffectMaterialId&& other) noexcept;
        ~EffectMaterialId() = default;
        EffectMaterialId& operator=(const EffectMaterialId& other);
        EffectMaterialId& operator=(EffectMaterialId&& other) noexcept;

        bool operator==(const EffectMaterialId& other) const;
        bool operator!=(const EffectMaterialId& other) const;

        bool isEqualSource(const EffectMaterialId& other) const;
        EffectMaterialId source() const { return EffectMaterialId(m_name); }

        const std::string& name() const { return m_name; }
        bool isSource() const { return m_instance_serial == 0; }

        class hash
        {
        public:
            size_t operator()(const EffectMaterialId& id) const
            {
                return std::hash<std::string>()(id.m_name) ^ std::hash<std::uint32_t>()(id.m_instance_serial);
            }
        };

    protected:
        std::string m_name;
        std::uint32_t m_instance_serial = 0;
    };
}

#endif // EFFECT_MATERIAL_ID_H
