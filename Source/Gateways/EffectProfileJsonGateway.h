/*********************************************************************
 * \file   EffectProfileJsonGateway.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_PROFILE_JSON_GATEWAY_H
#define EFFECT_PROFILE_JSON_GATEWAY_H

#define RAPIDJSON_HAS_STDSTRING 1

#include "GameEngine/EffectCompilingProfile.h"
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
    class EffectProfileJsonGateway
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
        EffectProfileJsonGateway();
        std::optional<Engine::EffectCompilingProfile> Deserialize(const std::string& json);

        void Cleanup();

    private:
        std::vector<VertexShaderGatewayMeta> DeserializeVertexShaderList(const rapidjson::Value& shader_list) const;
        std::vector<PixelShaderGatewayMeta> DeserializePixelShaderList(const rapidjson::Value& shader_list) const;
        std::vector<SamplerStateGatewayMeta> DeserializeSamplerStateList(const rapidjson::Value& sampler_list) const;
        std::vector<RasterizerStateGatewayMeta> DeserializeRasterizerStateList(const rapidjson::Value& rasterizer_list) const;
        std::vector<BlendStateGatewayMeta> DeserializeBlendStateList(const rapidjson::Value& blend_list) const;
        std::vector<DepthStateGatewayMeta> DeserializeDepthStateList(const rapidjson::Value& depth_list) const;

        std::vector<Engine::EffectTechniqueProfile> DeserializeTechniqueProfileList(const rapidjson::Value& tech_list) const;
        std::vector<Engine::EffectPassProfile> DeserializePassProfileList(const rapidjson::Value& pass_list) const;

        VertexShaderGatewayMeta DeserializeVertexShaderMeta(const rapidjson::Value& shader) const;
        PixelShaderGatewayMeta DeserializePixelShaderMeta(const rapidjson::Value& shader) const;
        SamplerStateGatewayMeta DeserializeSamplerStateMeta(const rapidjson::Value& state) const;
        RasterizerStateGatewayMeta DeserializeRasterizerStateMeta(const rapidjson::Value& state) const;
        BlendStateGatewayMeta DeserializeBlendStateMeta(const rapidjson::Value& state) const;
        DepthStateGatewayMeta DeserializeDepthStateMeta(const rapidjson::Value& state) const;

        Engine::EffectTechniqueProfile DeserializeTechniqueProfile(const rapidjson::Value& tech) const;
        Engine::EffectPassProfile DeserializePassProfile(const rapidjson::Value& pass) const;
        Engine::ShaderProgramPolicy DeserializeProgramPolicy(const std::string& name, const std::string& vtx_shader_name,
            const std::string& pix_shader_name) const;
        std::vector<Engine::EffectSamplerProfile> DeserializeSamplerProfileList(const rapidjson::Value& sampler_list) const;
        Engine::EffectAlphaBlendProfile FetchBlendStateProfile(const std::string& state_name) const;
        Engine::EffectRasterizerProfile FetchRasterizerStateProfile(const std::string& state_name) const;
        Engine::EffectDepthStencilProfile FetchDepthStateProfile(const std::string& state_name) const;
        std::string ReadShaderCode(const std::string& filename) const;

        MathLib::ColorRGBA DeserializeColorRGBA(const rapidjson::Value& value) const;
        Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpData DeserializeStencilOp(const rapidjson::Value& value) const;
        Graphics::IDeviceSamplerState::SamplerStateData::Filter DeReferenceFilter(const std::string& filter) const;
        Graphics::IDeviceSamplerState::SamplerStateData::AddressMode DeReferenceAddressMode(const std::string& address_mode) const;
        Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc DeReferenceSamplerCompareFunc(const std::string& compare_func) const;
        Graphics::IDeviceRasterizerState::RasterizerStateData::FillMode DeReferenceFillMode(const std::string& fill_mode) const;
        Graphics::IDeviceRasterizerState::RasterizerStateData::BackfaceCullMode DeReferenceCullMode(const std::string& cull_mode) const;
        Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType DeReferenceBlendType(const std::string& blend_type) const;
        Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc DeReferenceDepthCompareFunc(const std::string& compare_func) const;
        Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode DeReferenceStencilOpCode(const std::string& op_code) const;
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
        const std::unordered_map<std::string, Graphics::IDeviceSamplerState::SamplerStateData::CompareFunc> m_samplerCompReference{
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
        const std::unordered_map<std::string, Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType> m_blendTypeReference{
            {"Disable", Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType::Blend_Disable},
            {"Additive", Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType::Blend_Additive},
            {"Opaque", Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType::Blend_Opaque},
            {"Transparent", Graphics::IDeviceAlphaBlendState::BlendStateData::BlendType::Blend_Transparent},
        };
        const std::unordered_map<std::string, Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc> m_depthCompReference{
            {"Never", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::Never},
            {"Always", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::Always},
            {"Equal", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::Equal},
            {"Greater", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::Greater},
            {"GreaterEqual", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::GreaterEqual},
            {"Less", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::Less},
            {"LessEqual", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::LessEqual},
            {"NotEqual", Graphics::IDeviceDepthStencilState::DepthStencilData::CompareFunc::NotEqual},
        };
        const std::unordered_map<std::string, Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode> m_stencilOpReference{
            {"Keep", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Keep},
            {"Decr", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Decr},
            {"Decr_Sat", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Decr_Sat},
            {"Incr", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Incr},
            {"Incr_Sat", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Incr_Sat},
            {"Invert", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Invert},
            {"Replace", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Replace},
            {"Zero", Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpCode::Zero},
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
