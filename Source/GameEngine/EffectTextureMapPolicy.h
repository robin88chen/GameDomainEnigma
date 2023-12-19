/*********************************************************************
 * \file   EffectTextureMapPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _EFFECT_TEXTURE_MAP_POLICY_H
#define _EFFECT_TEXTURE_MAP_POLICY_H

#include "TexturePolicies.h"
#include <string>
#include <optional>
#include <tuple>
#include <vector>

namespace Enigma::Engine
{
    class EffectTextureMapPolicy
    {
    public:
        using TextureTuplePolicy = std::tuple<std::string, TexturePolicy, std::optional<unsigned>>;
    public:
        EffectTextureMapPolicy() = default;

        [[nodiscard]] const std::vector<TextureTuplePolicy>& textureTuplePolicies() const { return m_tuplePolicies; }
        std::vector<TextureTuplePolicy>& textureTuplePolicies() { return m_tuplePolicies; }

    protected:
        std::vector<TextureTuplePolicy> m_tuplePolicies;
    };
}


#endif // _EFFECT_TEXTURE_MAP_POLICY_H
