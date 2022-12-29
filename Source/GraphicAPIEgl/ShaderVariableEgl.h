/*********************************************************************
 * \file   ShaderVariableEgl.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_VARIABLE_EGL_H
#define SHADER_VARIABLE_EGL_H

#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GraphicKernel/ITexture.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"
#endif

namespace Enigma::Devices
{
    using error = std::error_code;

    class ShaderVariableEgl_Base : public Graphics::IShaderVariable
    {
    public:
        ShaderVariableEgl_Base(GLuint program, const std::string& name, const std::string& semantic);
        ShaderVariableEgl_Base(const ShaderVariableEgl_Base&) = delete;
        ShaderVariableEgl_Base(ShaderVariableEgl_Base&&) = delete;
        virtual ~ShaderVariableEgl_Base();
        ShaderVariableEgl_Base& operator=(const ShaderVariableEgl_Base&) = delete;
        ShaderVariableEgl_Base& operator=(ShaderVariableEgl_Base&&) = delete;

        virtual void SetValue(std::any data) override {};
        virtual void SetValues(std::any data_array, unsigned int count) override {};
    protected:
        GLuint m_program;
    };

    class ShaderVariableEgl_Matrix : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Matrix(GLuint program, const std::string& name, const std::string& semantic,
            unsigned int dimension, unsigned int elements);
        ShaderVariableEgl_Matrix(const ShaderVariableEgl_Matrix&) = delete;
        ShaderVariableEgl_Matrix(ShaderVariableEgl_Matrix&&) = delete;
        virtual ~ShaderVariableEgl_Matrix();
        ShaderVariableEgl_Matrix& operator=(const ShaderVariableEgl_Matrix&) = delete;
        ShaderVariableEgl_Matrix& operator=(ShaderVariableEgl_Matrix&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

        virtual error Apply() override;

    protected:
        std::vector<float> m_values;
        unsigned int m_dimension;
        unsigned int m_numElements;
    };

    class ShaderVariableEgl_Texture : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Texture(GLuint program, const std::string& name, const std::string& semantic, unsigned int bindSlot);
        ShaderVariableEgl_Texture(const ShaderVariableEgl_Texture&) = delete;
        ShaderVariableEgl_Texture(ShaderVariableEgl_Texture&&) = delete;
        virtual ~ShaderVariableEgl_Texture();
        ShaderVariableEgl_Texture& operator=(const ShaderVariableEgl_Texture&) = delete;
        ShaderVariableEgl_Texture& operator=(ShaderVariableEgl_Texture&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;

    protected:
        Graphics::ITexturePtr m_texture;
        std::optional<unsigned int> m_indexMultiTexture;
        unsigned int m_bindSlot;
    };

    class ShaderVariableEgl_Sampler : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Sampler(GLuint program, const std::string& name, const std::string& semantic, unsigned int bindSlot);
        ShaderVariableEgl_Sampler(const ShaderVariableEgl_Sampler&) = delete;
        ShaderVariableEgl_Sampler(ShaderVariableEgl_Sampler&&) = delete;
        virtual ~ShaderVariableEgl_Sampler();
        ShaderVariableEgl_Sampler& operator=(const ShaderVariableEgl_Sampler&) = delete;
        ShaderVariableEgl_Sampler& operator=(ShaderVariableEgl_Sampler&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;

    protected:
        Graphics::IDeviceSamplerStatePtr m_sampler;
        unsigned int m_bindSlot;
    };

    class ShaderVariableEgl_Vector : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Vector(GLuint program, const std::string& name, const std::string& semantic,
            unsigned int dimension, unsigned int elements);
        ShaderVariableEgl_Vector(const ShaderVariableEgl_Vector&) = delete;
        ShaderVariableEgl_Vector(ShaderVariableEgl_Vector&&) = delete;
        virtual ~ShaderVariableEgl_Vector();
        ShaderVariableEgl_Vector& operator=(const ShaderVariableEgl_Vector&) = delete;
        ShaderVariableEgl_Vector& operator=(ShaderVariableEgl_Vector&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;
    protected:
        std::vector<float> m_values;
        unsigned int m_dimension;
        unsigned int m_numElements;
    };

    class ShaderVariableEgl_Boolean : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Boolean(GLuint program, const std::string& name, const std::string& semantic, unsigned int elements);
        ShaderVariableEgl_Boolean(const ShaderVariableEgl_Boolean&) = delete;
        ShaderVariableEgl_Boolean(ShaderVariableEgl_Boolean&&) = delete;
        virtual ~ShaderVariableEgl_Boolean();
        ShaderVariableEgl_Boolean& operator=(const ShaderVariableEgl_Boolean&) = delete;
        ShaderVariableEgl_Boolean& operator=(ShaderVariableEgl_Boolean&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;

    protected:
        std::vector<int> m_values;
        unsigned int m_numElements;
    };

    class ShaderVariableEgl_Float : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Float(GLuint program, const std::string& name, const std::string& semantic, unsigned int elements);
        ShaderVariableEgl_Float(const ShaderVariableEgl_Float&) = delete;
        ShaderVariableEgl_Float(ShaderVariableEgl_Float&&) = delete;
        virtual ~ShaderVariableEgl_Float();
        ShaderVariableEgl_Float& operator=(const ShaderVariableEgl_Float&) = delete;
        ShaderVariableEgl_Float& operator=(ShaderVariableEgl_Float&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;

    protected:
        std::vector <float> m_values;
        unsigned int m_numElements;
    };

    class ShaderVariableEgl_Int : public ShaderVariableEgl_Base
    {
    public:
        ShaderVariableEgl_Int(GLuint program, const std::string& name, const std::string& semantic, unsigned int elements);
        ShaderVariableEgl_Int(const ShaderVariableEgl_Boolean&) = delete;
        ShaderVariableEgl_Int(ShaderVariableEgl_Boolean&&) = delete;
        virtual ~ShaderVariableEgl_Int();
        ShaderVariableEgl_Int& operator=(const ShaderVariableEgl_Int&) = delete;
        ShaderVariableEgl_Int& operator=(ShaderVariableEgl_Int&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;
        virtual error Apply() override;

    protected:
        std::vector<int> m_values;
        unsigned int m_numElements;
    };

}

#endif // SHADER_VARIABLE_EGL_H
