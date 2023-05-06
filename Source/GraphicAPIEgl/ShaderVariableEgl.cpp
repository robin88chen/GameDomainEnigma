#include "ShaderVariableEgl.h"
#include "TextureEgl.h"
#include "MultiTextureEgl.h"
#include "DeviceSamplerStateEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "MathLib/Matrix2.h"
#include "MathLib/Matrix3.h"
#include "MathLib/Matrix4.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

ShaderVariableEgl_Base::ShaderVariableEgl_Base(GLuint program, const std::string& name,
    const std::string& semantic) : IShaderVariable(name, semantic)
{
    m_program = program;
}

ShaderVariableEgl_Base::~ShaderVariableEgl_Base()
{
}

ShaderVariableEgl_Matrix::ShaderVariableEgl_Matrix(GLuint program, const std::string& name, const std::string& semantic,
    unsigned dimension, unsigned elements) : ShaderVariableEgl_Base(program, name, semantic)
{
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_dimension = dimension;
    m_values.resize(elements * dimension * dimension, 0.0f);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::matrixValue));
}

ShaderVariableEgl_Matrix::~ShaderVariableEgl_Matrix()
{
}

void ShaderVariableEgl_Matrix::SetValue(std::any data)
{
    try
    {
        if (m_dimension == 4)
        {
            MathLib::Matrix4 mx = std::any_cast<MathLib::Matrix4>(data);
            memcpy(&m_values[0], static_cast<float*>(mx), m_dimension * m_dimension * sizeof(float));
        }
        if (m_dimension == 3)
        {
            MathLib::Matrix3 mx = std::any_cast<MathLib::Matrix3>(data);
            memcpy(&m_values[0], static_cast<float*>(mx), m_dimension * m_dimension * sizeof(float));
        }
        if (m_dimension == 2)
        {
            MathLib::Matrix2 mx = std::any_cast<MathLib::Matrix2>(data);
            memcpy(&m_values[0], static_cast<float*>(mx), m_dimension * m_dimension * sizeof(float));
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to matrix%d : %s",
            m_name.c_str(), m_semantic.c_str(), m_dimension, e.what());
    }
}

void ShaderVariableEgl_Matrix::SetValues(std::any data_array, unsigned count)
{
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        if (m_dimension == 4)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Matrix4>))
            {
                auto mx = std::any_cast<std::vector<MathLib::Matrix4>>(data_array);
                memcpy(&m_values[0], &(mx[0]), count * m_dimension * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Matrix4* mx = std::any_cast<MathLib::Matrix4*>(data_array);
                memcpy(&m_values[0], mx, count * m_dimension * m_dimension * sizeof(float));
            }
        }
        if (m_dimension == 3)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Matrix3>))
            {
                auto mx = std::any_cast<std::vector<MathLib::Matrix3>>(data_array);
                memcpy(&m_values[0], &(mx[0]), count * m_dimension * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Matrix3* mx = std::any_cast<MathLib::Matrix3*>(data_array);
                memcpy(&m_values[0], mx, count * m_dimension * m_dimension * sizeof(float));
            }
        }
        if (m_dimension == 2)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Matrix2>))
            {
                auto mx = std::any_cast<std::vector<MathLib::Matrix2>>(data_array);
                memcpy(&m_values[0], &(mx[0]), count * m_dimension * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Matrix2* mx = std::any_cast<MathLib::Matrix2*>(data_array);
                memcpy(&m_values[0], mx, count * m_dimension * m_dimension * sizeof(float));
            }
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to matrix%d array : %s",
            m_name.c_str(), m_semantic.c_str(), m_dimension, e.what());
    }
}

error ShaderVariableEgl_Matrix::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if (m_dimension == 4)
    {
        glUniformMatrix4fv(loc, static_cast<GLsizei>(m_numElements), GL_FALSE, &m_values[0]);
    }
    else if (m_dimension == 3)
    {
        glUniformMatrix3fv(loc, static_cast<GLsizei>(m_numElements), GL_FALSE, &m_values[0]);
    }
    else if (m_dimension == 2)
    {
        glUniformMatrix2fv(loc, static_cast<GLsizei>(m_numElements), GL_FALSE, &m_values[0]);
    }
    return ErrorCode::ok;
}

ShaderVariableEgl_Texture::ShaderVariableEgl_Texture(GLuint program, const std::string& name,
    const std::string& semantic, unsigned bindSlot) : ShaderVariableEgl_Base(program, name, semantic)
{
    m_bindSlot = bindSlot;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::textureObject));
}

ShaderVariableEgl_Texture::~ShaderVariableEgl_Texture()
{
}

void ShaderVariableEgl_Texture::SetValue(std::any data)
{
    try
    {
        auto value = std::any_cast<TextureVarTuple>(data);
        m_texture = std::get<Graphics::ITexturePtr>(value);
        m_indexMultiTexture = std::get<std::optional<unsigned int>>(value);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to texture : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

void ShaderVariableEgl_Texture::SetValues(std::any data_array, unsigned count)
{
    Platforms::Debug::Printf("shader variable of texture array not support!!");
}

error ShaderVariableEgl_Texture::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if ((m_texture) && (!m_texture->IsMultiTexture()))
    {
        TextureEgl* tex_egl = dynamic_cast<TextureEgl*>(m_texture.get());
        assert(tex_egl);
        glActiveTexture(GL_TEXTURE0 + m_bindSlot);
        if (!tex_egl->IsCubeTexture())
        {
            glBindTexture(GL_TEXTURE_2D, tex_egl->GetTextureHandle());
        }
        else
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex_egl->GetTextureHandle());
        }
    }
    else if ((m_texture) && (m_texture->IsMultiTexture()) &&(m_indexMultiTexture))
    {
        MultiTextureEgl* tex_egl = dynamic_cast<MultiTextureEgl*>(m_texture.get());
        assert(tex_egl);
        glActiveTexture(GL_TEXTURE0 + m_bindSlot);
        glBindTexture(GL_TEXTURE_2D, tex_egl->GetTextureHandle(m_indexMultiTexture.value()));
    }
    glUniform1i(loc, static_cast<GLint>(m_bindSlot));
    return ErrorCode::ok;
}

ShaderVariableEgl_Sampler::ShaderVariableEgl_Sampler(GLuint program, const std::string& name,
    const std::string& semantic, unsigned bindSlot) : ShaderVariableEgl_Base(program, name, semantic)
{
    m_bindSlot = bindSlot;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::samplerState));
}

ShaderVariableEgl_Sampler::~ShaderVariableEgl_Sampler()
{
}

void ShaderVariableEgl_Sampler::SetValue(std::any data)
{
    try
    {
        m_sampler = std::any_cast<Graphics::IDeviceSamplerStatePtr>(data);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to sampler : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

void ShaderVariableEgl_Sampler::SetValues(std::any data_array, unsigned count)
{
    Platforms::Debug::Printf("shader variable of sampler array not support!!");
}

error ShaderVariableEgl_Sampler::Apply()
{
    if (!m_sampler) return ErrorCode::nullSamplerState;
    DeviceSamplerStateEgl* sampler_egl = dynamic_cast<DeviceSamplerStateEgl*>(m_sampler.get());
    assert(sampler_egl);

    return sampler_egl->BindToShader(m_bindSlot);
}

ShaderVariableEgl_Vector::ShaderVariableEgl_Vector(GLuint program, const std::string& name, const std::string& semantic,
    unsigned dimension, unsigned elements) : ShaderVariableEgl_Base(program, name, semantic)
{
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_dimension = dimension;
    m_values.resize(elements * dimension, 0.0f);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::vectorValue));
}

ShaderVariableEgl_Vector::~ShaderVariableEgl_Vector()
{
}

void ShaderVariableEgl_Vector::SetValue(std::any data)
{
    try
    {
        if (m_dimension == 4)
        {
            MathLib::Vector4 vec = std::any_cast<MathLib::Vector4>(data);
            memcpy(&m_values[0], static_cast<float*>(vec), m_dimension * sizeof(float));
        }
        if (m_dimension == 3)
        {
            MathLib::Vector3 vec = std::any_cast<MathLib::Vector3>(data);
            memcpy(&m_values[0], static_cast<float*>(vec), m_dimension * sizeof(float));
        }
        if (m_dimension == 2)
        {
            MathLib::Vector2 vec = std::any_cast<MathLib::Vector2>(data);
            memcpy(&m_values[0], static_cast<float*>(vec), m_dimension * sizeof(float));
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to vector%d : %s",
            m_name.c_str(), m_semantic.c_str(), m_dimension, e.what());
    }
}

void ShaderVariableEgl_Vector::SetValues(std::any data_array, unsigned count)
{
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        if (m_dimension == 4)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Vector4>))
            {
                auto vec = std::any_cast<std::vector<MathLib::Vector4>>(data_array);
                memcpy(&m_values[0], &(vec[0]), count * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Vector4* vec = std::any_cast<MathLib::Vector4*>(data_array);
                memcpy(&m_values[0], vec, count * m_dimension * sizeof(float));
            }
        }
        if (m_dimension == 3)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Vector3>))
            {
                auto vec = std::any_cast<std::vector<MathLib::Vector3>>(data_array);
                memcpy(&m_values[0], &(vec[0]), count * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Vector3* vec = std::any_cast<MathLib::Vector3*>(data_array);
                memcpy(&m_values[0], vec, count * m_dimension * sizeof(float));
            }
        }
        if (m_dimension == 2)
        {
            if (data_array.type() == typeid(std::vector<MathLib::Vector2>))
            {
                auto vec = std::any_cast<std::vector<MathLib::Vector2>>(data_array);
                memcpy(&m_values[0], &(vec[0]), count * m_dimension * sizeof(float));
            }
            else
            {
                MathLib::Vector2* vec = std::any_cast<MathLib::Vector2*>(data_array);
                memcpy(&m_values[0], vec, count * m_dimension * sizeof(float));
            }
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to vector%d array : %s",
            m_name.c_str(), m_semantic.c_str(), m_dimension, e.what());
    }
}

error ShaderVariableEgl_Vector::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if (m_dimension == 4)
    {
        glUniform4fv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    else if (m_dimension == 3)
    {
        glUniform3fv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    else if (m_dimension == 2)
    {
        glUniform2fv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    return ErrorCode::ok;
}

ShaderVariableEgl_Boolean::ShaderVariableEgl_Boolean(GLuint program, const std::string& name,
    const std::string& semantic, unsigned elements) : ShaderVariableEgl_Base(program, name, semantic)
{
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_values.resize(elements, 0);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::booleanValue));
}

ShaderVariableEgl_Boolean::~ShaderVariableEgl_Boolean()
{
}

void ShaderVariableEgl_Boolean::SetValue(std::any data)
{
    try
    {
        bool b = std::any_cast<bool>(data);
        m_values[0] = b ? 1 : 0;
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to bool : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

void ShaderVariableEgl_Boolean::SetValues(std::any data_array, unsigned count)
{
    try
    {
        if (count == 0) count = 1;
        if (count > m_numElements) count = m_numElements;
        if (data_array.type() == typeid(std::vector<bool>))
        {
            auto bs = std::any_cast<std::vector<bool>>(data_array);
            for (unsigned i = 0; i < count; i++)
            {
                m_values[i] = bs[i] ? 1 : 0;
            }
        }
        else
        {
            auto bs = std::any_cast<bool*>(data_array);
            for (unsigned i = 0; i < count; i++)
            {
                m_values[i] = bs[i] ? 1 : 0;
            }
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to bool array : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

error ShaderVariableEgl_Boolean::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if (m_numElements <= 1)
    {
        glUniform1i(loc, m_values[0]);
    }
    else
    {
        glUniform1iv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    return ErrorCode::ok;
}

ShaderVariableEgl_Float::ShaderVariableEgl_Float(GLuint program, const std::string& name, const std::string& semantic,
    unsigned elements) : ShaderVariableEgl_Base(program, name, semantic)
{
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_values.resize(elements, 0.0f);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::floatValue));
}

ShaderVariableEgl_Float::~ShaderVariableEgl_Float()
{
}

void ShaderVariableEgl_Float::SetValue(std::any data)
{
    try
    {
        m_values[0] = std::any_cast<float>(data);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to float : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

void ShaderVariableEgl_Float::SetValues(std::any data_array, unsigned count)
{
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        if (data_array.type() == typeid(std::vector<float>))
        {
            auto datas = std::any_cast<std::vector<float>>(data_array);
            memcpy(&m_values[0], &(datas[0]), count * sizeof(float));
        }
        else
        {
            float* datas = std::any_cast<float*>(data_array);
            memcpy(&m_values[0], datas, count * sizeof(float));
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to float array : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

error ShaderVariableEgl_Float::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if (m_numElements <= 1)
    {
        glUniform1f(loc, m_values[0]);
    }
    else
    {
        glUniform1fv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    return ErrorCode::ok;
}

ShaderVariableEgl_Int::ShaderVariableEgl_Int(GLuint program, const std::string& name, const std::string& semantic,
    unsigned elements) : ShaderVariableEgl_Base(program, name, semantic)
{
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_values.resize(elements, 0);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::intValue));
}

ShaderVariableEgl_Int::~ShaderVariableEgl_Int()
{
}

void ShaderVariableEgl_Int::SetValue(std::any data)
{
    try
    {
        m_values[0] = std::any_cast<int>(data);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to int : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

void ShaderVariableEgl_Int::SetValues(std::any data_array, unsigned count)
{
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        if (data_array.type() == typeid(std::vector<int>))
        {
            auto datas = std::any_cast<std::vector<int>>(data_array);
            memcpy(&m_values[0], &(datas[0]), count * sizeof(int));
        }
        else
        {
            int* datas = std::any_cast<int*>(data_array);
            memcpy(&m_values[0], datas, count * sizeof(int));
        }
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast to int array : %s", m_name.c_str(), m_semantic.c_str(), e.what());
    }
}

error ShaderVariableEgl_Int::Apply()
{
    assert(glIsProgram(m_program));
    GLint loc = glGetUniformLocation(m_program, m_name.c_str());
    if (loc < 0) return ErrorCode::shaderVarLocation;
    if (m_numElements <= 1)
    {
        glUniform1i(loc, m_values[0]);
    }
    else
    {
        glUniform1iv(loc, static_cast<GLsizei>(m_numElements), &m_values[0]);
    }
    return ErrorCode::ok;
}
