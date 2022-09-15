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
#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/IDeviceRasterizerState.h"
#include "GraphicKernel/IDeviceAlphaBlendState.h"
#include "GraphicKernel/IDeviceDepthStencilState.h"
#include "MathLib/ColorRGBA.h"
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
        struct SamplerStateGatewayMeta
        {
            std::string m_name;
            Graphics::IDeviceSamplerState::SamplerStateData m_stateData;
        };
        struct RasterizerStateGatewayMeta
        {
            std::string m_name;
            Graphics::IDeviceRasterizerState::RasterizerStateData m_stateData;
        };
        struct BlendStateGatewayMeta
        {
            std::string m_name;
            Graphics::IDeviceAlphaBlendState::BlendStateData m_stateData;
        };
        struct DepthStateGatewayMeta
        {
            std::string m_name;
            Graphics::IDeviceDepthStencilState::DepthStencilData m_stateData;
        };
    public:
        EffectPolicyJsonGateway();
        std::optional<Engine::EffectCompilingPolicy> Deserialize(const std::string& json);

    private:
        std::vector<VertexShaderGatewayMeta> DeserializeVertexShaderList(const rapidjson::Value& shader_list) const;
        std::vector<PixelShaderGatewayMeta> DeserializePixelShaderList(const rapidjson::Value& shader_list) const;
        std::vector<SamplerStateGatewayMeta> DeserializeSamplerStateList(const rapidjson::Value& sampler_list) const;
        std::vector<RasterizerStateGatewayMeta> DeserializeRasterizerStateList(const rapidjson::Value& rasterizer_list) const;
        std::vector<BlendStateGatewayMeta> DeserializeBlendStateList(const rapidjson::Value& sampler_list) const;
        std::vector<DepthStateGatewayMeta> DeserializeDepthStateList(const rapidjson::Value& sampler_list) const;

        VertexShaderGatewayMeta DeserializeVertexShaderMeta(const rapidjson::Value& shader) const;
        PixelShaderGatewayMeta DeserializePixelShaderMeta(const rapidjson::Value& shader) const;
        SamplerStateGatewayMeta DeserializeSamplerStateMeta(const rapidjson::Value& state) const;
        RasterizerStateGatewayMeta DeserializeRasterizerStateMeta(const rapidjson::Value& state) const;
        BlendStateGatewayMeta DeserializeBlendStateMeta(const rapidjson::Value& state) const;
        DepthStateGatewayMeta DeserializeDepthStateMeta(const rapidjson::Value& state) const;

        MathLib::ColorRGBA DeserializeColorRGBA(const rapidjson::Value& value) const;
    private:
        const std::unordered_map<std::string, Graphics::IGraphicAPI::APIVersion> m_apiReference {
            {"dx9", Graphics::IGraphicAPI::APIVersion::API_Dx9}, {"dx11", Graphics::IGraphicAPI::APIVersion::API_Dx11},
            {"egl", Graphics::IGraphicAPI::APIVersion::API_EGL}
        };
        const std::unordered_map<std::string, Graphics::IDeviceSamplerState::SamplerStateData::Filter> m_filterReference {
            {"None", Graphics::IDeviceSamplerState::SamplerStateData::Filter::None},
            {"Linear", Graphics::IDeviceSamplerState::SamplerStateData::Filter::Linear},
            {"Point", Graphics::IDeviceSamplerState::SamplerStateData::Filter::Point},
        };
        const std::unordered_map<std::string, Graphics::IDeviceSamplerState::SamplerStateData::AddressMode> m_addressModeReference {
            {"Wrap", Graphics::IDeviceSamplerState::SamplerStateData::AddressMode::Wrap},
            {"Clamp", Graphics::IDeviceSamplerState::SamplerStateData::AddressMode::Clamp},
            {"Mirror", Graphics::IDeviceSamplerState::SamplerStateData::AddressMode::Mirror},
            {"Border", Graphics::IDeviceSamplerState::SamplerStateData::AddressMode::Border},
        };
        const std::unordered_map<std::string, Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc> m_compFuncReference{
            {"Never", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::Never},
            {"Always", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::Always},
            {"Equal", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::Equal},
            {"Greater", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::Greater},
            {"GreaterEqual", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::GreaterEqual},
            {"Less", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::Less},
            {"LessEqual", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::LessEqual},
            {"NotEqual", Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc::NotEqual},
        };
        const std::unordered_map<std::string, Graphics::IDeviceRasterizerState::RasterizerStateData::FillMode> m_fillModeReference{
            {"Fill", Graphics::IDeviceRasterizerState::RasterizerStateData::FillMode::Fill},
            {"Wireframe", Graphics::IDeviceRasterizerState::RasterizerStateData::FillMode::Wireframe},
            {"Point", Graphics::IDeviceRasterizerState::RasterizerStateData::FillMode::Point},
        };
        const std::unordered_map<std::string, Graphics::IDeviceRasterizerState::RasterizerStateData::BackfaceCullMode> m_cullModeReference{
            {"None", Graphics::IDeviceRasterizerState::RasterizerStateData::BackfaceCullMode::Cull_None},
            {"CW", Graphics::IDeviceRasterizerState::RasterizerStateData::BackfaceCullMode::Cull_CW},
            {"CCW", Graphics::IDeviceRasterizerState::RasterizerStateData::BackfaceCullMode::Cull_CCW},
        };
        std::vector<VertexShaderGatewayMeta> m_vertexShaderGateways;
        std::vector<PixelShaderGatewayMeta> m_pixelShaderGateways;
        std::vector<SamplerStateGatewayMeta> m_samplerStateGateways;
        std::vector<RasterizerStateGatewayMeta> m_rasterizerStateGateways;
        std::vector<BlendStateGatewayMeta> m_blendStateGateways;
        std::vector<DepthStateGatewayMeta> m_depthStateGateways;
    };
}

#endif // EFFECT_POLICY_JSON_GATEWAY_H
