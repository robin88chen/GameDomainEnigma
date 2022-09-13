#include "EffectPolicyJsonGateway.h"
#include "Frameworks/StringFormat.h"
#include "Platforms/PlatformLayer.h"
#include "rapidjson/document.h"

using namespace Enigma::Gateways;
using namespace Enigma::Engine;

#define VERTEX_SHADER_LIST_TOKEN "VertexShaderList"

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

    return std::nullopt;
}

std::vector<EffectPolicyJsonGateway::VertexShaderGatewayMeta> EffectPolicyJsonGateway::DeserializeVertexShaderList(
    const rapidjson::Value& shader_list)
{
    return std::vector<VertexShaderGatewayMeta>{};
}
