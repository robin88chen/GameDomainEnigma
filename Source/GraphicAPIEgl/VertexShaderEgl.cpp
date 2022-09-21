#include "VertexShaderEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/TokenVector.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

VertexShaderEgl::VertexShaderEgl(const std::string& name) : IVertexShader(name)
{
    m_shader = 0;
    m_varSemanticTable.clear();
    m_texSamplerTable.clear();
}

VertexShaderEgl::~VertexShaderEgl()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
    m_varSemanticTable.clear();
    m_texSamplerTable.clear();
}

std::string VertexShaderEgl::GetVarSemantic(const std::string& var_name)
{
    if (!m_varSemanticTable.empty())
    {
        auto iter = m_varSemanticTable.find(var_name);
        if (iter != m_varSemanticTable.end())
        {
            return iter->second;
        }
    }
    return "";
}

std::string VertexShaderEgl::GetSamplerStateName(const std::string& tex_var_name)
{
    if (!m_texSamplerTable.empty())
    {
        auto iter = m_texSamplerTable.find(tex_var_name);
        if (iter != m_texSamplerTable.end())
        {
            return iter->second;
        }
    }
    return "";
}

void VertexShaderEgl::ParseSemanticTable(const std::string& code)
{
    m_varSemanticTable.clear();
#include "ParseSemanticTable.inl"
}

void VertexShaderEgl::ParseSamplerStateTable(const std::string& code)
{
    m_texSamplerTable.clear();
#include "ParseSamplerStateTable.inl"
}

