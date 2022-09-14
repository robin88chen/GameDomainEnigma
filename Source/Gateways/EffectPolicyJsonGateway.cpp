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
