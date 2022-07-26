#include "PixelShaderEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/TokenVector.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

PixelShaderEgl::PixelShaderEgl(const std::string& name) : IPixelShader(name)
{
    m_shader = 0;
    m_varSemanticTable.clear();
    m_texSamplerTable.clear();
}

PixelShaderEgl::~PixelShaderEgl()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
    m_varSemanticTable.clear();
    m_texSamplerTable.clear();
}

error PixelShaderEgl::CompileCode(const std::string& code, const std::string& profile, const std::string& entry)
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
    m_varSemanticTable.clear();
    m_texSamplerTable.clear();
    m_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* code_src = code.c_str();
    glShaderSource(m_shader, 1, &code_src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(m_shader);
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLogLen = 0;
        glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetShaderInfoLog(m_shader, infoLogLen, NULL, infoLog);
                Platforms::Debug::ErrorPrintf("Could not compile pixel shader:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        return ErrorCode::compileShader;
    }
    ParseSemanticTable(code);
    ParseSamplerStateTable(code);

    m_hasCompiled = true;

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::PixelShaderCompiled(m_name) });
    return ErrorCode::ok;
}

std::string PixelShaderEgl::GetVarSemantic(const std::string& var_name)
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

std::string PixelShaderEgl::GetSamplerStateName(const std::string& tex_var_name)
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

void PixelShaderEgl::ParseSemanticTable(const std::string& code)
{
    m_varSemanticTable.clear();
#include "ParseSemanticTable.inl"
}

void PixelShaderEgl::ParseSamplerStateTable(const std::string& code)
{
    m_texSamplerTable.clear();
#include "ParseSamplerStateTable.inl"
}

