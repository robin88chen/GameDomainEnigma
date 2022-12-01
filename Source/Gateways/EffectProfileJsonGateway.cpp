#include "EffectProfileJsonGateway.h"
#include "Frameworks/StringFormat.h"
#include "Platforms/PlatformLayer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "rapidjson/document.h"

using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::FileSystem;

#define VERTEX_SHADER_LIST_TOKEN "VertexShaderList"
#define PIXEL_SHADER_LIST_TOKEN "PixelShaderList"
#define NAME_TOKEN  "Name"
#define FILE_TOKEN  "File"
#define PROFILE_TOKEN   "Profile"
#define ENTRY_TOKEN "Entry"
#define VTX_DECL_TOKEN "VtxDecl"
#define API_TOKEN   "API"
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
#define BLEND_STATE_LIST_TOKEN "BlendStateList"
#define BLEND_TYPES_TOKEN "BlendTypes"
#define DEPTH_STATE_LIST_TOKEN "DepthStateList"
#define DEPTH_TEST_TOKEN "DepthTest"
#define DEPTH_WRITE_TOKEN "DepthWrite"
#define DEPTH_FUNC_TOKEN "DepthFunc"
#define STENCIL_TEST_TOKEN "StencilTest"
#define STENCIL_WRITE_TOKEN "StencilWrite"
#define STENCIL_READ_TOKEN "StencilRead"
#define STENCIL_FRONT_OP_TOKEN "StencilFrontFace"
#define STENCIL_BACK_OP_TOKEN "StencilBackFace"
#define STENCIL_FUNC_TOKEN "StencilFunc"
#define STENCIL_FAIL_TOKEN "OnFail"
#define STENCIL_PASS_TOKEN "OnPass"
#define STENCIL_DEPTH_FAIL_TOKEN "OnDepthFail"
#define STENCIL_REF_VALUE_TOKEN "StencilRefValue"
#define EFFECT_PASS_ARRAY_TOKEN "EffectPassArray"
#define TECHNIQUE_LIST_TOKEN    "TechniqueList"
#define PROGRAM_TOKEN "Program"
#define VERTEX_SHADER_TOKEN "VertexShader"
#define PIXEL_SHADER_TOKEN "PixelShader"
#define SAMPLER_STATE_TOKEN "SamplerState"
#define BLEND_STATE_TOKEN "BlendState"
#define DEPTH_STATE_TOKEN "DepthState"
#define RASTERIZER_STATE_TOKEN "RasterizerState"
#define SAMPLER_TOKEN "Sampler"
#define VAR_TOKEN "Var"

EffectProfileJsonGateway::EffectProfileJsonGateway()
{
}

std::optional<EffectCompilingProfile> EffectProfileJsonGateway::Deserialize(const std::string& json)
{
    rapidjson::Document json_doc;
    json_doc.Parse<0>(json.c_str());
    if (FATAL_LOG_EXPR(json_doc.HasParseError()))
    {
        std::string ss = string_format("error %d, @ %d", json_doc.GetParseError(), json_doc.GetErrorOffset());
        LOG(Info, ss);
        return std::nullopt;
    }
    if (!json_doc.HasMember(TECHNIQUE_LIST_TOKEN))
    {
        std::string ss = string_format("effect doesn't have technique list");
        LOG(Info, ss);
        return std::nullopt;
    }
    m_vertexShaderGateways = DeserializeVertexShaderList(json_doc[VERTEX_SHADER_LIST_TOKEN]);
    m_pixelShaderGateways = DeserializePixelShaderList(json_doc[PIXEL_SHADER_LIST_TOKEN]);
    m_samplerStateGateways = DeserializeSamplerStateList(json_doc[SAMPLER_STATE_LIST_TOKEN]);
    m_rasterizerStateGateways = DeserializeRasterizerStateList(json_doc[RASTERIZER_STATE_LIST_TOKEN]);
    m_blendStateGateways = DeserializeBlendStateList(json_doc[BLEND_STATE_LIST_TOKEN]);
    m_depthStateGateways = DeserializeDepthStateList(json_doc[DEPTH_STATE_LIST_TOKEN]);

    EffectCompilingProfile profile;
    profile.m_name = json_doc[NAME_TOKEN].GetString();
    profile.m_techniques = DeserializeTechniqueProfileList(json_doc[TECHNIQUE_LIST_TOKEN]);

    return profile;
}

void EffectProfileJsonGateway::Cleanup()
{
    m_vertexShaderGateways.clear();
    m_pixelShaderGateways.clear();
    m_samplerStateGateways.clear();
    m_rasterizerStateGateways.clear();
    m_blendStateGateways.clear();
    m_depthStateGateways.clear();
}

std::vector<EffectProfileJsonGateway::VertexShaderGatewayMeta> EffectProfileJsonGateway::DeserializeVertexShaderList(
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

std::vector<EffectProfileJsonGateway::PixelShaderGatewayMeta> EffectProfileJsonGateway::DeserializePixelShaderList(
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

std::vector<EffectProfileJsonGateway::SamplerStateGatewayMeta> EffectProfileJsonGateway::DeserializeSamplerStateList(const rapidjson::Value& sampler_list) const
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

std::vector<EffectProfileJsonGateway::RasterizerStateGatewayMeta> EffectProfileJsonGateway::DeserializeRasterizerStateList(const rapidjson::Value& rasterizer_list) const
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

std::vector<EffectProfileJsonGateway::BlendStateGatewayMeta> EffectProfileJsonGateway::DeserializeBlendStateList(const rapidjson::Value& blend_list) const
{
    auto state_metas = std::vector<BlendStateGatewayMeta>{};
    if (FATAL_LOG_EXPR(!blend_list.IsArray()))
    {
        std::string ss = string_format("blend state list is not a array");
        LOG(Info, ss);
        return state_metas;
    }
    for (auto blend = blend_list.GetArray().Begin(); blend != blend_list.GetArray().End(); ++blend)
    {
        state_metas.emplace_back(DeserializeBlendStateMeta(*blend));
    }

    return state_metas;
}

std::vector<EffectProfileJsonGateway::DepthStateGatewayMeta> EffectProfileJsonGateway::DeserializeDepthStateList(const rapidjson::Value& depth_list) const
{
    auto state_metas = std::vector<DepthStateGatewayMeta>{};
    if (FATAL_LOG_EXPR(!depth_list.IsArray()))
    {
        std::string ss = string_format("depth state list is not a array");
        LOG(Info, ss);
        return state_metas;
    }
    for (auto depth = depth_list.GetArray().Begin(); depth != depth_list.GetArray().End(); ++depth)
    {
        state_metas.emplace_back(DeserializeDepthStateMeta(*depth));
    }

    return state_metas;
}

std::vector<EffectTechniqueProfile> EffectProfileJsonGateway::DeserializeTechniqueProfileList(const rapidjson::Value& tech_list) const
{
    auto profiles = std::vector<EffectTechniqueProfile>{};
    if (FATAL_LOG_EXPR(!tech_list.IsArray()))
    {
        std::string ss = string_format("technique list is not a array");
        LOG(Info, ss);
        return profiles;
    }
    for (auto tech = tech_list.GetArray().Begin(); tech != tech_list.GetArray().End(); ++tech)
    {
        profiles.emplace_back(DeserializeTechniqueProfile(*tech));
    }

    return profiles;
}

std::vector<EffectPassProfile> EffectProfileJsonGateway::DeserializePassProfileList(const rapidjson::Value& pass_list) const
{
    auto profiles = std::vector<EffectPassProfile>{};
    if (FATAL_LOG_EXPR(!pass_list.IsArray()))
    {
        std::string ss = string_format("pass list is not a array");
        LOG(Info, ss);
        return profiles;
    }
    for (auto pass = pass_list.GetArray().Begin(); pass != pass_list.GetArray().End(); ++pass)
    {
        profiles.emplace_back(DeserializePassProfile(*pass));
    }

    return profiles;
}

EffectProfileJsonGateway::VertexShaderGatewayMeta EffectProfileJsonGateway::DeserializeVertexShaderMeta(const rapidjson::Value& shader) const
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

EffectProfileJsonGateway::PixelShaderGatewayMeta EffectProfileJsonGateway::DeserializePixelShaderMeta(const rapidjson::Value& shader) const
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

EffectProfileJsonGateway::SamplerStateGatewayMeta EffectProfileJsonGateway::DeserializeSamplerStateMeta(const rapidjson::Value& state) const
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
        meta.m_stateData.m_cmpFunc = m_samplerCompReference.at(state[COMPARE_FUNC_TOKEN].GetString());
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

EffectProfileJsonGateway::RasterizerStateGatewayMeta EffectProfileJsonGateway::DeserializeRasterizerStateMeta(const rapidjson::Value& state) const
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

EffectProfileJsonGateway::BlendStateGatewayMeta EffectProfileJsonGateway::DeserializeBlendStateMeta(const rapidjson::Value& state) const
{
    assert(!state.IsNull());

    BlendStateGatewayMeta meta;
    meta.m_name = state[NAME_TOKEN].GetString();
    if (FATAL_LOG_EXPR(!state[BLEND_TYPES_TOKEN].IsArray()))
    {
        std::string ss = string_format("blend types is not a array");
        LOG(Info, ss);
        return meta;
    }
    for (unsigned int i = 0; i < state[BLEND_TYPES_TOKEN].Capacity(); i++)
    {
        meta.m_stateData.m_blendType[i] = m_blendTypeReference.at(state[BLEND_TYPES_TOKEN][i].GetString());
    }
    meta.m_stateData.m_isMultiBlendTarget = state[BLEND_TYPES_TOKEN].Capacity() > 1;

    return meta;
}

EffectProfileJsonGateway::DepthStateGatewayMeta EffectProfileJsonGateway::DeserializeDepthStateMeta(const rapidjson::Value& state) const
{
    assert(!state.IsNull());

    DepthStateGatewayMeta meta;
    meta.m_name = state[NAME_TOKEN].GetString();
    if (state.HasMember(DEPTH_TEST_TOKEN))
    {
        meta.m_stateData.m_isDepthTestEnable = state[DEPTH_TEST_TOKEN].GetBool();
    }
    if (state.HasMember(DEPTH_WRITE_TOKEN))
    {
        meta.m_stateData.m_isDepthWriteEnable = state[DEPTH_WRITE_TOKEN].GetBool();
    }
    if (state.HasMember(STENCIL_TEST_TOKEN))
    {
        meta.m_stateData.m_isStencilTestEnable = state[STENCIL_TEST_TOKEN].GetBool();
    }
    if (state.HasMember(STENCIL_WRITE_TOKEN))
    {
        meta.m_stateData.m_stencilWriteMask = static_cast<unsigned char>(state[STENCIL_WRITE_TOKEN].GetUint());
    }
    if (state.HasMember(STENCIL_READ_TOKEN))
    {
        meta.m_stateData.m_stencilReadMask = static_cast<unsigned char>(state[STENCIL_WRITE_TOKEN].GetUint());
    }
    if (state.HasMember(DEPTH_FUNC_TOKEN))
    {
        meta.m_stateData.m_depthCompare = m_depthCompReference.at(state[DEPTH_FUNC_TOKEN].GetString());
    }
    if (state.HasMember(STENCIL_REF_VALUE_TOKEN))
    {
        meta.m_stateData.m_stencilRefValue = state[STENCIL_REF_VALUE_TOKEN].GetUint();
    }
    if (state.HasMember(STENCIL_FRONT_OP_TOKEN))
    {
        meta.m_stateData.m_frontFaceOp = DeserializeStencilOp(state[STENCIL_FRONT_OP_TOKEN]);
    }
    if (state.HasMember(STENCIL_BACK_OP_TOKEN))
    {
        meta.m_stateData.m_backFaceOp = DeserializeStencilOp(state[STENCIL_BACK_OP_TOKEN]);
    }

    return meta;
}

EffectTechniqueProfile EffectProfileJsonGateway::DeserializeTechniqueProfile(const rapidjson::Value& tech) const
{
    assert(!tech.IsNull());
    EffectTechniqueProfile profile;
    profile.m_name = tech[NAME_TOKEN].GetString();
    profile.m_passes = DeserializePassProfileList(tech[EFFECT_PASS_ARRAY_TOKEN]);

    return profile;
}

EffectPassProfile EffectProfileJsonGateway::DeserializePassProfile(const rapidjson::Value& pass) const
{
    assert(!pass.IsNull());
    EffectPassProfile profile;
    profile.m_name = pass[NAME_TOKEN].GetString();
    profile.m_program = DeserializeProgramPolicy(pass[PROGRAM_TOKEN].GetString(),
        pass[VERTEX_SHADER_TOKEN].GetString(), pass[PIXEL_SHADER_TOKEN].GetString());
    if (pass.HasMember(SAMPLER_STATE_TOKEN))
        profile.m_samplers = DeserializeSamplerProfileList(pass[SAMPLER_STATE_TOKEN]);
    if (pass.HasMember(BLEND_STATE_TOKEN))
        profile.m_blend = FetchBlendStateProfile(pass[BLEND_STATE_TOKEN].GetString());
    if (pass.HasMember(RASTERIZER_STATE_TOKEN))
        profile.m_rasterizer = FetchRasterizerStateProfile(pass[RASTERIZER_STATE_TOKEN].GetString());
    if (pass.HasMember(DEPTH_STATE_TOKEN))
        profile.m_depth = FetchDepthStateProfile(pass[DEPTH_STATE_TOKEN].GetString());

    return profile;
}

ShaderProgramPolicy EffectProfileJsonGateway::DeserializeProgramPolicy(const std::string& name,
    const std::string& vtx_shader_name, const std::string& pix_shader_name) const
{
    Graphics::IGraphicAPI::APIVersion api = Graphics::IGraphicAPI::Instance()->GetAPIVersion();
    ShaderProgramPolicy policy;
    policy.m_programName = name;
    auto vtx_it = std::find_if(m_vertexShaderGateways.begin(), m_vertexShaderGateways.end(), [=](auto g)
        { return g.m_shaderName == vtx_shader_name && g.m_api == api; });
    if (vtx_it != m_vertexShaderGateways.end())
    {
        policy.m_vtxShaderName = vtx_shader_name;
        policy.m_vtxFormatCode = vtx_it->m_formatCode;
        policy.m_vtxLayoutName = vtx_it->m_layoutName;
        policy.m_vtxShaderCode.m_profile = vtx_it->m_profile;
        policy.m_vtxShaderCode.m_entry = vtx_it->m_entry;
        policy.m_vtxShaderCode.m_code = ReadShaderCode(vtx_it->m_filename);
    }
    auto pix_it = std::find_if(m_pixelShaderGateways.begin(), m_pixelShaderGateways.end(), [=](auto g)
        { return g.m_shaderName == pix_shader_name && g.m_api == api; });
    if (pix_it != m_pixelShaderGateways.end())
    {
        policy.m_pixShaderName = pix_shader_name;
        policy.m_pixShaderCode.m_profile = pix_it->m_profile;
        policy.m_pixShaderCode.m_entry = pix_it->m_entry;
        policy.m_pixShaderCode.m_code = ReadShaderCode(pix_it->m_filename);
    }
    return policy;
}

std::vector<EffectSamplerProfile> EffectProfileJsonGateway::DeserializeSamplerProfileList(const rapidjson::Value& sampler_list) const
{
    assert(!sampler_list.IsNull());
    auto profiles = std::vector<EffectSamplerProfile>{};
    if (FATAL_LOG_EXPR(!sampler_list.IsArray()))
    {
        std::string ss = string_format("sampler list is not a array");
        LOG(Info, ss);
        return profiles;
    }
    for (auto samp = sampler_list.GetArray().Begin(); samp != sampler_list.GetArray().End(); ++samp)
    {
        auto samp_it = std::find_if(m_samplerStateGateways.begin(), m_samplerStateGateways.end(), [=](auto g)
        {
            return g.m_name == (*samp)[SAMPLER_TOKEN].GetString();
        });
        if (samp_it != m_samplerStateGateways.end())
        {
            profiles.emplace_back(EffectSamplerProfile{
                samp_it->m_name, samp_it->m_stateData, (*samp)[VAR_TOKEN].GetString() });
        }
    }
    return profiles;
}

EffectAlphaBlendProfile EffectProfileJsonGateway::FetchBlendStateProfile(const std::string& state_name) const
{
    EffectAlphaBlendProfile profile;
    auto it = std::find_if(m_blendStateGateways.begin(), m_blendStateGateways.end(), [=](auto g)
        {
            return g.m_name == state_name;
        });
    if (FATAL_LOG_EXPR(it == m_blendStateGateways.end()))
    {
        std::string ss = string_format("can't find blend state %s", state_name.c_str());
        LOG(Info, ss);
        return profile;
    }
    profile.m_name = it->m_name;
    profile.m_data = it->m_stateData;
    return profile;
}

EffectRasterizerProfile EffectProfileJsonGateway::FetchRasterizerStateProfile(const std::string& state_name) const
{
    EffectRasterizerProfile profile;
    auto it = std::find_if(m_rasterizerStateGateways.begin(), m_rasterizerStateGateways.end(), [=](auto g)
        {
            return g.m_name == state_name;
        });
    if (FATAL_LOG_EXPR(it == m_rasterizerStateGateways.end()))
    {
        std::string ss = string_format("can't find rasterizer state %s", state_name.c_str());
        LOG(Info, ss);
        return profile;
    }
    profile.m_name = it->m_name;
    profile.m_data = it->m_stateData;
    return profile;
}

EffectDepthStencilProfile EffectProfileJsonGateway::FetchDepthStateProfile(const std::string& state_name) const
{
    EffectDepthStencilProfile profile;
    auto it = std::find_if(m_depthStateGateways.begin(), m_depthStateGateways.end(), [=](auto g)
        {
            return g.m_name == state_name;
        });
    if (FATAL_LOG_EXPR(it == m_depthStateGateways.end()))
    {
        std::string ss = string_format("can't find depth state %s", state_name.c_str());
        LOG(Info, ss);
        return profile;
    }
    profile.m_name = it->m_name;
    profile.m_data = it->m_stateData;
    return profile;
}

std::string EffectProfileJsonGateway::ReadShaderCode(const std::string& filename) const
{
    IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(Filename(filename), "rb");
    if (FATAL_LOG_EXPR(!iFile)) return "";
    size_t file_size = iFile->Size();
    if (FATAL_LOG_EXPR(file_size <= 0))
    {
        FileSystem::FileSystem::Instance()->CloseFile(iFile);
        return "";
    }

    auto code_buff = iFile->Read(0, file_size);
    if (!code_buff) return "";
    FileSystem::FileSystem::Instance()->CloseFile(iFile);
    return convert_to_string(code_buff.value(), file_size);
}

Enigma::MathLib::ColorRGBA EffectProfileJsonGateway::DeserializeColorRGBA(const rapidjson::Value& value) const
{
    if (!value.IsArray()) return MathLib::ColorRGBA{};
    auto values = value.GetArray();
    return MathLib::ColorRGBA{ values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat() };
}

Enigma::Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpData EffectProfileJsonGateway::DeserializeStencilOp(
    const rapidjson::Value& value) const
{
    Graphics::IDeviceDepthStencilState::DepthStencilData::StencilOpData data;
    if (value.HasMember(STENCIL_FUNC_TOKEN))
    {
        data.m_compare = m_depthCompReference.at(value[STENCIL_FUNC_TOKEN].GetString());
    }
    if (value.HasMember(STENCIL_FAIL_TOKEN))
    {
        data.m_failOp = m_stencilOpReference.at(value[STENCIL_FAIL_TOKEN].GetString());
    }
    if (value.HasMember(STENCIL_PASS_TOKEN))
    {
        data.m_passOp = m_stencilOpReference.at(value[STENCIL_PASS_TOKEN].GetString());
    }
    if (value.HasMember(STENCIL_DEPTH_FAIL_TOKEN))
    {
        data.m_depthFailOp = m_stencilOpReference.at(value[STENCIL_DEPTH_FAIL_TOKEN].GetString());
    }
    return data;
}
