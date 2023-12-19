/*********************************************************************
 * \file   EffectMaterialPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_POLICY_H
#define EFFECT_MATERIAL_POLICY_H

#include "EffectCompilingProfile.h"
#include "EffectCompilingProfileDeserializer.h"
#include <string>
#include <optional>
#include <memory>

namespace Enigma::Engine
{
    class EffectMaterialPolicy
    {
    public:
        EffectMaterialPolicy() = default;
        EffectMaterialPolicy(const std::string& name, const EffectCompilingProfile& profile) : m_name(name), m_profile(profile) {}
        EffectMaterialPolicy(const std::string& name, const std::string& deserialize_param,
            const std::shared_ptr<IEffectCompilingProfileDeserializer>& deserializer)
            : m_name(name), m_parameter(deserialize_param), m_deserializer(deserializer) {}

        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }

        [[nodiscard]] const std::optional<EffectCompilingProfile>& profile() const { return m_profile; }

        [[nodiscard]] const std::string& parameter() const { return m_parameter; }
        [[nodiscard]] const std::shared_ptr<IEffectCompilingProfileDeserializer>& deserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::optional<EffectCompilingProfile> m_profile;
        std::string m_parameter;
        std::shared_ptr<IEffectCompilingProfileDeserializer> m_deserializer;
    };
}

#endif // EFFECT_MATERIAL_POLICY_H
