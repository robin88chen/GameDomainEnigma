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
#include "GraphicKernel/IGraphicAPI.h"
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
            Graphics::IGraphicAPI::APIVersion m_api;
            std::string m_filename;
            std::string m_profile;
            std::string m_entry;
            std::string m_formatCode;
            std::string m_layoutName;
        };
        struct PixelShaderGatewayMeta
        {
            std::string m_shaderName;
            Graphics::IGraphicAPI::APIVersion m_api;
            std::string m_filename;
            std::string m_profile;
            std::string m_entry;
        };
    public:
        EffectPolicyJsonGateway();
        std::optional<Engine::EffectCompilingPolicy> Deserialize(const std::string& json);

    private:
        std::vector<VertexShaderGatewayMeta> DeserializeVertexShaderList(const rapidjson::Value& shader_list) const;
        std::vector<PixelShaderGatewayMeta> DeserializePixelShaderList(const rapidjson::Value& shader_list) const;

        VertexShaderGatewayMeta DeserializeVertexShaderMeta(const rapidjson::Value& shader) const;
        PixelShaderGatewayMeta DeserializePixelShaderMeta(const rapidjson::Value& shader) const;

    private:
        const std::unordered_map<std::string, Graphics::IGraphicAPI::APIVersion> m_apiReference {
            {"dx9", Graphics::IGraphicAPI::APIVersion::API_Dx9}, {"dx11", Graphics::IGraphicAPI::APIVersion::API_Dx11},
            {"egl", Graphics::IGraphicAPI::APIVersion::API_EGL}
        };
        std::vector<VertexShaderGatewayMeta> m_vertexShaderGateways;
        std::vector<PixelShaderGatewayMeta> m_pixelShaderGateways;
    };
}

#endif // EFFECT_POLICY_JSON_GATEWAY_H
