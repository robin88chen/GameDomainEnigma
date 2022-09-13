/*********************************************************************
 * \file   EffectPolicyJsonGateway.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_POLICY_JSON_GATEWAY_H
#define EFFECT_POLICY_JSON_GATEWAY_H

#define RAPIDJSON_HAS_STDSTRING 1

#include "GameEngine/EffectCompilingPolicies.h"
#include "rapidjson/document.h"
#include <optional>

namespace Enigma::Gateways
{
    class EffectPolicyJsonGateway
    {
    private:
        struct VertexShaderGatewayMeta
        {
            std::string m_shaderName;
            std::string m_formatCode;
            Engine::ShaderCodeProfile m_shaderCode;
            std::string m_layoutName;
        };
    public:
        EffectPolicyJsonGateway();
        std::optional<Engine::EffectCompilingPolicy> Deserialize(const std::string& json);

    private:
        std::vector<VertexShaderGatewayMeta> DeserializeVertexShaderList(const rapidjson::Value& shader_list);

    private:
        std::vector<VertexShaderGatewayMeta> m_vertexShaderGateways;
    };
}

#endif // EFFECT_POLICY_JSON_GATEWAY_H
