#include "ShaderCompilerEgl.h"
#include "VertexShaderEgl.h"
#include "PixelShaderEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"

using namespace Enigma::Graphics;
using namespace Enigma::Devices;

ShaderCompilerEgl::ShaderCompilerEgl()
{
    
}

ShaderCompilerEgl::~ShaderCompilerEgl()
{
    
}

Enigma::Devices::error ShaderCompilerEgl::CompileVertexShader(const Graphics::IVertexShaderPtr& shader, 
    const std::string& code, const std::string& profile, const std::string& entry)
{
    auto shader_egl = std::dynamic_pointer_cast<VertexShaderEgl, IVertexShader>(shader);
    if (shader_egl->m_shader)
    {
        glDeleteShader(shader_egl->m_shader);
    }
    shader_egl->m_varSemanticTable.clear();
    shader_egl->m_texSamplerTable.clear();
    shader_egl->m_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* code_src = code.c_str();
    glShaderSource(shader_egl->m_shader, 1, &code_src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader_egl->m_shader);
    glGetShaderiv(shader_egl->m_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLogLen = 0;
        glGetShaderiv(shader_egl->m_shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetShaderInfoLog(shader_egl->m_shader, infoLogLen, NULL, infoLog);
                Platforms::Debug::ErrorPrintf("Could not compile vertex shader:\n%s\n", infoLog);
                Frameworks::EventPublisher::Post(std::make_shared<Graphics::VertexShaderCompileFailed>(shader_egl->m_name, infoLog));
                free(infoLog);
            }
        }
        return ErrorCode::compileShader;
    }
    shader_egl->ParseSemanticTable(code);
    shader_egl->ParseSamplerStateTable(code);

    shader_egl->m_hasCompiled = true;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::VertexShaderCompiled>(shader_egl->m_name));

    return ErrorCode::ok;
}

Enigma::Devices::error ShaderCompilerEgl::CompilePixelShader(const Graphics::IPixelShaderPtr& shader, 
    const std::string& code, const std::string& profile, const std::string& entry)
{
    auto shader_egl = std::dynamic_pointer_cast<PixelShaderEgl, IPixelShader>(shader);
    if (shader_egl->m_shader)
    {
        glDeleteShader(shader_egl->m_shader);
    }
    shader_egl->m_varSemanticTable.clear();
    shader_egl->m_texSamplerTable.clear();
    shader_egl->m_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* code_src = code.c_str();
    glShaderSource(shader_egl->m_shader, 1, &code_src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader_egl->m_shader);
    glGetShaderiv(shader_egl->m_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLogLen = 0;
        glGetShaderiv(shader_egl->m_shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetShaderInfoLog(shader_egl->m_shader, infoLogLen, NULL, infoLog);
                Platforms::Debug::ErrorPrintf("Could not compile pixel shader:\n%s\n", infoLog);
                Frameworks::EventPublisher::Post(std::make_shared<Graphics::PixelShaderCompileFailed>(shader_egl->m_name, infoLog));
                free(infoLog);
            }
        }
        return ErrorCode::compileShader;
    }
    shader_egl->ParseSemanticTable(code);
    shader_egl->ParseSamplerStateTable(code);

    shader_egl->m_hasCompiled = true;

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::PixelShaderCompiled>(shader_egl->m_name));
    return ErrorCode::ok;
}
