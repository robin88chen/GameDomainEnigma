#include "ShaderProgramEgl.h"
#include "GraphicAPIEgl.h"
#include "VertexShaderEgl.h"
#include "PixelShaderEgl.h"
#include "VertexDeclarationEgl.h"
#include "ShaderVariableEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

ShaderProgramEgl::ShaderProgramEgl(const std::string& name,const Graphics::IVertexShaderPtr& vtx_shader, 
    const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl)
        : IShaderProgram(name, vtx_shader, pix_shader, vtx_decl)
{
    assert((m_vtxShader) && (m_pixShader));

    m_hasLinked = false;
    m_program = glCreateProgram();
    VertexShaderEgl* vs = dynamic_cast<VertexShaderEgl*>(m_vtxShader.get());
    assert(vs);
    glAttachShader(m_program, vs->GetShader());
    PixelShaderEgl* ps = dynamic_cast<PixelShaderEgl*>(m_pixShader.get());
    assert(ps);
    glAttachShader(m_program, ps->GetShader());
    GLint shader_count = 0;
    glGetProgramiv(m_program, GL_ATTACHED_SHADERS, &shader_count);
    if (shader_count >= 2)
    {
        LinkShaders();
        RetrieveShaderVariables();
    }
}

ShaderProgramEgl::~ShaderProgramEgl()
{
    if (m_program)
    {
        glDeleteProgram(m_program);
    }
    ClearVariableArray();
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramEgl::GetVariableByName(const std::string& name)
{
    if (m_variableArray.empty()) return nullptr;
    for (auto var : m_variableArray)
    {
        if (!var) continue;
        if (var->GetVariableName() == name) return var;
    }
    return nullptr;
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramEgl::GetVariableBySemantic(const std::string& semantic)
{
    if (m_variableArray.empty()) return nullptr;
    for (auto var : m_variableArray)
    {
        if (!var) continue;
        if (var->GetVariableSemantic() == semantic) return var;
    }
    return nullptr;
}

unsigned int ShaderProgramEgl::GetVariableCount()
{
    return m_variableArray.size();
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramEgl::GetVariableByIndex(unsigned int index)
{
    if (index >= m_variableArray.size()) return nullptr;
    return m_variableArray[index];
}

error ShaderProgramEgl::CommitVariables()
{
    if (m_variableArray.size() > 0)
    {
        for (auto var : m_variableArray)
        {
            if (var)
            {
                var->Commit();
            }
        }
    }
    return ErrorCode::ok;
}

error ShaderProgramEgl::ApplyVariables()
{
    if (m_variableArray.size() > 0)
    {
        for (auto var : m_variableArray)
        {
            if (var)
            {
                error er = var->Apply();
                if (er) return er;
            }
        }
    }
    return ErrorCode::ok;
}

future_error ShaderProgramEgl::AsyncApplyVariables()
{
    future_error er = make_future_err(ErrorCode::ok);
    if (m_variableArray.size() > 0)
    {
        for (auto var : m_variableArray)
        {
            if (var)
            {
                er = var->AsyncApply();
            }
        }
    }
    return er;
}

void ShaderProgramEgl::LinkShaders()
{
    assert(m_program != 0);
    glLinkProgram(m_program);
    GLint linked = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        Platforms::Debug::ErrorPrintf("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetProgramInfoLog(m_program, infoLogLen, NULL, infoLog);
                Platforms::Debug::ErrorPrintf("Could not link program:\n%s\n", infoLog);
                Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderProgramLinkFailed>(m_name, infoLog));
                free(infoLog);
            }
        }
        m_hasLinked = false;
        return;
    }
    m_hasLinked = true;
}

void ShaderProgramEgl::RetrieveShaderVariables()
{
    assert(m_program != 0);
    ClearVariableArray();
    GLint linked = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked) return;
    GLint act_uniforms = 0;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &act_uniforms);
    if (act_uniforms == 0) return;
    GLint max_name_length;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);
    GLchar* name_buff = memalloc(GLchar, max_name_length);
    unsigned int sampler_slot_index = 0;
    auto vtx_shader = std::dynamic_pointer_cast<VertexShaderEgl, Graphics::IVertexShader>(m_vtxShader);
    auto pix_shader = std::dynamic_pointer_cast<PixelShaderEgl, Graphics::IPixelShader>(m_pixShader);
    if ((!vtx_shader) || (!pix_shader)) return;
    for (int i = 0; i < act_uniforms; i++)
    {
        GLsizei name_length;
        GLint uniform_size;
        GLenum uniform_type;
        glGetActiveUniform(m_program, i, max_name_length, &name_length, &uniform_size, &uniform_type, name_buff);
        if (name_length == 0) continue;
        if ((uniform_type == GL_SAMPLER_2D) || (uniform_type == GL_SAMPLER_CUBE))
        {
            std::string semantic = vtx_shader->GetVarSemantic(name_buff);
            if (semantic.length() == 0) semantic = pix_shader->GetVarSemantic(name_buff);
            std::string samp_name = vtx_shader->GetSamplerStateName(name_buff);
            if (samp_name.length() == 0) samp_name = pix_shader->GetSamplerStateName(name_buff);
            std::string samp_semantic;
            if (samp_name.length() > 0)
            {
                samp_semantic = vtx_shader->GetVarSemantic(samp_name);
                if (samp_semantic.length() == 0) samp_semantic = pix_shader->GetVarSemantic(samp_name);
            }
            AppendShaderSamplerVariable(name_buff, semantic, samp_name, samp_semantic,
                sampler_slot_index, uniform_type, uniform_size);
        }
        else
        {
            std::string semantic = vtx_shader->GetVarSemantic(name_buff);
            if (semantic.length() == 0) semantic = pix_shader->GetVarSemantic(name_buff);
            AppendShaderVariable(name_buff, semantic, uniform_type, uniform_size);
        }
    }
}

void ShaderProgramEgl::ClearVariableArray()
{
    m_variableArray.clear();
}

void ShaderProgramEgl::AppendShaderVariable(const std::string& name, const std::string& semantic,
    GLenum type, int elements)
{
    if (type == GL_FLOAT)
    {
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Float(
            m_program, name, semantic, elements) };
        m_variableArray.push_back(shader_var);
    }
    else if (type == GL_INT)
    {
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Int(
            m_program, name, semantic, elements) };
        m_variableArray.push_back(shader_var);
    }
    else if (type == GL_BOOL)
    {
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Boolean(
            m_program, name, semantic, elements) };
        m_variableArray.push_back(shader_var);
    }
    else if ((type == GL_FLOAT_VEC2) || (type == GL_FLOAT_VEC3) || (type == GL_FLOAT_VEC4))
    {
        int dimension = (unsigned int)type - (unsigned int)GL_FLOAT_VEC2 + 2;
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Vector(
            m_program, name, semantic, dimension, elements) };
        m_variableArray.push_back(shader_var);
    }
    else if ((type == GL_FLOAT_MAT2) || (type == GL_FLOAT_MAT3) || (type == GL_FLOAT_MAT4))
    {
        int dimension = (unsigned int)type - (unsigned int)GL_FLOAT_MAT2 + 2;
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Matrix(
            m_program, name, semantic, dimension, elements) };
        m_variableArray.push_back(shader_var);
    }
}

void ShaderProgramEgl::AppendShaderSamplerVariable(const std::string& name, const std::string& semantic,
    const std::string& samp_name, const std::string& samp_semantic, unsigned int& slot_index, GLenum type, int elements)
{
    if ((type == GL_SAMPLER_2D) || (type == GL_SAMPLER_CUBE))
    {
        Graphics::IShaderVariablePtr shader_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Texture(
            m_program, name, semantic, slot_index) };
        m_variableArray.push_back(shader_var);
        if (samp_name.length() != 0)
        {
            Graphics::IShaderVariablePtr samp_var = Graphics::IShaderVariablePtr{ menew ShaderVariableEgl_Sampler(
                m_program, samp_name, samp_semantic, slot_index) };
            m_variableArray.push_back(samp_var);
        }
        slot_index++;
    }
}
