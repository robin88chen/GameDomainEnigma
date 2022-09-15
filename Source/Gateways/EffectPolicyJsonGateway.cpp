#include "EffectPolicyJsonGateway.h"
#include "Frameworks/StringFormat.h"
#include "Platforms/PlatformLayer.h"
#include "rapidjson/document.h"

using namespace Enigma::Gateways;
using namespace Enigma::Engine;

#define VERTEX_SHADER_LIST_TOKEN "VertexShaderList"
#define PIXEL_SHADER_LIST_TOKEN "PixelShaderList"
#define NAME_TOKEN	"Name"
#define FILE_TOKEN	"File"
#define PROFILE_TOKEN	"Profile"
#define ENTRY_TOKEN	"Entry"
#define VTX_DECL_TOKEN "VtxDecl"
#define API_TOKEN	"API"
#define SAMPLER_STATE_LIST_TOKEN "SamplerStateList"
#define ADDRESS_U_TOKEN "AddressU"
#define ADDRESS_V_TOKEN "AddressV"
#define ADDRESS_W_TOKEN "AddressW"
#define MIN_FILTER_TOKEN "MinFilter"
#define MAG_FILTER_TOKEN "MagFilter"
#define MIP_FILTER_TOKEN "MipFilter"
#define COMPARE_FUNC_TOKEN "CmpFunc"
#define BORDER_COLOR_TOKEN "BorderColor"
#define ANISOTROPIC_TOKEN "Anisotropic"
#define LOD_BIAS_TOKEN "LodBias"
#define MIN_LOD_TOKEN "MinLod"
#define MAX_LOD_TOKEN "MaxLod"
#define RASTERIZER_STATE_LIST_TOKEN "RasterizerStateList"
#define FILLMODE_TOKEN "FillMode"
#define CULLMODE_TOKEN "CullMode"
#define DEPTH_BIAS_TOKEN "DepthBias"
#define DEPTH_BIAS_CLAMP_TOKEN "DepthBiasClamp"
#define SLOPE_SCALE_DEPTH_BIAS_TOKEN "SlopeScaleDepthBias"

EffectPolicyJsonGateway::EffectPolicyJsonGateway()
{
}

std::optional<EffectCompilingPolicy> EffectPolicyJsonGateway::Deserialize(const std::string& json)
{
    rapidjson::Document json_doc;
    json_doc.Parse<0>(json.c_str());
    if (FATAL_LOG_EXPR(json_doc.HasParseError()))
    {
        std::string ss = string_format("error %d, @ %d", json_doc.GetParseError(), json_doc.GetErrorOffset());
        LOG(Info, ss);
        return std::nullopt;
    }
    m_vertexShaderGateways = DeserializeVertexShaderList(json_doc[VERTEX_SHADER_LIST_TOKEN]);
    m_pixelShaderGateways = DeserializePixelShaderList(json_doc[PIXEL_SHADER_LIST_TOKEN]);
    m_samplerStateGateways = DeserializeSamplerStateList(json_doc[SAMPLER_STATE_LIST_TOKEN]);
    m_rasterizerStateGateways = DeserializeRasterizerStateList(json_doc[RASTERIZER_STATE_LIST_TOKEN]);

    return std::nullopt;
}

std::vector<EffectPolicyJsonGateway::VertexShaderGatewayMeta> EffectPolicyJsonGateway::DeserializeVertexShaderList(
    const rapidjson::Value& shader_list) const
{
    auto shader_metas = std::vector<VertexShaderGatewayMeta>{};
    if (FATAL_LOG_EXPR(!shader_list.IsArray()))
    {
        std::string ss = string_format("vertex shader list is not a array");
        LOG(Info, ss);
        return shader_metas;
    }
    for (auto shader_it = shader_list.GetArray().Begin(); shader_it != shader_list.GetArray().End(); ++shader_it)
    {
        shader_metas.emplace_back(DeserializeVertexShaderMeta(*shader_it));
    }

    return shader_metas;
}

std::vector<EffectPolicyJsonGateway::PixelShaderGatewayMeta> EffectPolicyJsonGateway::DeserializePixelShaderList(
    const rapidjson::Value& shader_list) const
{
    auto shader_metas = std::vector<PixelShaderGatewayMeta>{};
    if (FATAL_LOG_EXPR(!shader_list.IsArray()))
    {
        std::string ss = string_format("pixel shader list is not a array");
        LOG(Info, ss);
        return shader_metas;
    }
    for (auto shader_it = shader_list.GetArray().Begin(); shader_it != shader_list.GetArray().End(); ++shader_it)
    {
        shader_metas.emplace_back(DeserializePixelShaderMeta(*shader_it));
    }

    return shader_metas;
}

std::vector<EffectPolicyJsonGateway::SamplerStateGatewayMeta> EffectPolicyJsonGateway::DeserializeSamplerStateList(const rapidjson::Value& sampler_list) const
{
    auto state_metas = std::vector<SamplerStateGatewayMeta>{};
    if (FATAL_LOG_EXPR(!sampler_list.IsArray()))
    {
        std::string ss = string_format("sampler state list is not a array");
        LOG(Info, ss);
        return state_metas;
    }
    for (auto sampler = sampler_list.GetArray().Begin(); sampler != sampler_list.GetArray().End(); ++sampler)
    {
        state_metas.emplace_back(DeserializeSamplerStateMeta(*sampler));
    }

    return state_metas;
}

std::vector<EffectPolicyJsonGateway::RasterizerStateGatewayMeta> EffectPolicyJsonGateway::DeserializeRasterizerStateList(const rapidjson::Value& rasterizer_list) const
{
    auto state_metas = std::vector<RasterizerStateGatewayMeta>{};
    if (FATAL_LOG_EXPR(!rasterizer_list.IsArray()))
    {
        std::string ss = string_format("rasterizer state list is not a array");
        LOG(Info, ss);
        return state_metas;
    }
    for (auto rasterizer = rasterizer_list.GetArray().Begin(); rasterizer != rasterizer_list.GetArray().End(); ++rasterizer)
    {
        state_metas.emplace_back(DeserializeRasterizerStateMeta(*rasterizer));
    }

    return state_metas;
}

EffectPolicyJsonGateway::VertexShaderGatewayMeta EffectPolicyJsonGateway::DeserializeVertexShaderMeta(const rapidjson::Value& shader) const
{
    assert(!shader.IsNull());

    VertexShaderGatewayMeta meta;
    meta.m_shaderName = shader[NAME_TOKEN].GetString();
    meta.m_api = m_apiReference.at(shader[API_TOKEN].GetString());
    meta.m_filename = shader[FILE_TOKEN].GetString();
    meta.m_profile = shader[PROFILE_TOKEN].GetString();
    meta.m_entry = shader[ENTRY_TOKEN].GetString();
    meta.m_formatCode = shader[VTX_DECL_TOKEN].GetString();
    meta.m_layoutName = meta.m_shaderName + ":" + meta.m_formatCode;
    return meta;
}

EffectPolicyJsonGateway::PixelShaderGatewayMeta EffectPolicyJsonGateway::DeserializePixelShaderMeta(const rapidjson::Value& shader) const
{
    assert(!shader.IsNull());

    PixelShaderGatewayMeta meta;
    meta.m_shaderName = shader[NAME_TOKEN].GetString();
    meta.m_api = m_apiReference.at(shader[API_TOKEN].GetString());
    meta.m_filename = shader[FILE_TOKEN].GetString();
    meta.m_profile = shader[PROFILE_TOKEN].GetString();
    meta.m_entry = shader[ENTRY_TOKEN].GetString();
    return meta;
}

EffectPolicyJsonGateway::SamplerStateGatewayMeta EffectPolicyJsonGateway::DeserializeSamplerStateMeta(const rapidjson::Value& state) const
{
    assert(!state.IsNull());

    SamplerStateGatewayMeta meta;
    meta.m_name = state[NAME_TOKEN].GetString();
    if (state.HasMember(ADDRESS_U_TOKEN))
        meta.m_stateData.m_addressModeU = m_addressModeReference.at(state[ADDRESS_U_TOKEN].GetString());
    if (state.HasMember(ADDRESS_V_TOKEN))
        meta.m_stateData.m_addressModeV = m_addressModeReference.at(state[ADDRESS_V_TOKEN].GetString());
    if (state.HasMember(ADDRESS_W_TOKEN))
        meta.m_stateData.m_addressModeW = m_addressModeReference.at(state[ADDRESS_W_TOKEN].GetString());
    if (state.HasMember(MAG_FILTER_TOKEN))
        meta.m_stateData.m_magFilter = m_filterReference.at(state[MAG_FILTER_TOKEN].GetString());
    if (state.HasMember(MIN_FILTER_TOKEN))
        meta.m_stateData.m_minFilter = m_filterReference.at(state[MIN_FILTER_TOKEN].GetString());
    if (state.HasMember(MIP_FILTER_TOKEN))
        meta.m_stateData.m_mipFilter = m_filterReference.at(state[MIP_FILTER_TOKEN].GetString());
    if (state.HasMember(COMPARE_FUNC_TOKEN))
        meta.m_stateData.m_cmpFunc = m_compFuncReference.at(state[COMPARE_FUNC_TOKEN].GetString());
    if (state.HasMember(BORDER_COLOR_TOKEN))
        meta.m_stateData.m_borderColor = DeserializeColorRGBA(state[BORDER_COLOR_TOKEN]);
    if (state.HasMember(ANISOTROPIC_TOKEN))
        meta.m_stateData.m_levelAnisotropic = static_cast<unsigned char>(state[ANISOTROPIC_TOKEN].GetInt());
    if (state.HasMember(LOD_BIAS_TOKEN))
        meta.m_stateData.m_mipmapLODBias = state[LOD_BIAS_TOKEN].GetFloat();
    if (state.HasMember(MIN_LOD_TOKEN))
        meta.m_stateData.m_minLOD = state[MIN_LOD_TOKEN].GetFloat();
    if (state.HasMember(MAX_LOD_TOKEN))
        meta.m_stateData.m_maxLOD = state[MAX_LOD_TOKEN].GetFloat();

    return meta;
}

EffectPolicyJsonGateway::RasterizerStateGatewayMeta EffectPolicyJsonGateway::DeserializeRasterizerStateMeta(const rapidjson::Value& state) const
{
    assert(!state.IsNull());

    RasterizerStateGatewayMeta meta;
    meta.m_name = state[NAME_TOKEN].GetString();
    if (state.HasMember(FILLMODE_TOKEN))
        meta.m_stateData.m_fillMode = m_fillModeReference.at(state[FILLMODE_TOKEN].GetString());
    if (state.HasMember(CULLMODE_TOKEN))
        meta.m_stateData.m_cullMode = m_cullModeReference.at(state[CULLMODE_TOKEN].GetString());
    if (state.HasMember(DEPTH_BIAS_TOKEN))
        meta.m_stateData.m_depthBias = state[DEPTH_BIAS_TOKEN].GetFloat();
    if (state.HasMember(DEPTH_BIAS_CLAMP_TOKEN))
        meta.m_stateData.m_depthBiasClamp = state[DEPTH_BIAS_CLAMP_TOKEN].GetFloat();
    if (state.HasMember(SLOPE_SCALE_DEPTH_BIAS_TOKEN))
        meta.m_stateData.m_slopeScaledDepthBias = state[SLOPE_SCALE_DEPTH_BIAS_TOKEN].GetFloat();

    return meta;
}

Enigma::MathLib::ColorRGBA EffectPolicyJsonGateway::DeserializeColorRGBA(const rapidjson::Value& value) const
{
    if (!value.IsArray()) return MathLib::ColorRGBA{};
    auto values = value.GetArray();
    return MathLib::ColorRGBA{ values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat() };
}

