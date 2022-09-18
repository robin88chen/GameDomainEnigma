/*********************************************************************
 * \file   ShaderProgramEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_PROGRAM_EGL_H
#define SHADER_PROGRAM_EGL_H

#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IShaderVariable.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class ShaderProgramEgl : public Graphics::IShaderProgram
    {
    public:
        ShaderProgramEgl(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader, 
            const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl);
        ShaderProgramEgl(const ShaderProgramEgl&) = delete;
        ShaderProgramEgl(ShaderProgramEgl&&) = delete;
        virtual ~ShaderProgramEgl();
        ShaderProgramEgl& operator=(const ShaderProgramEgl&) = delete;
        ShaderProgramEgl& operator=(ShaderProgramEgl&&) = delete;

        virtual Graphics::IShaderVariablePtr GetVariableByName(const std::string& name) override;
        virtual Graphics::IShaderVariablePtr GetVariableBySemantic(const std::string& semantic) override;
        virtual unsigned int GetVariableCount() override;
        virtual Graphics::IShaderVariablePtr GetVariableByIndex(unsigned int index) override;

        //virtual error CommitVariables() override;
        
        GLuint GetProgram() const { return m_program; }
        bool HasLinked() const { return m_hasLinked; }
    protected:
        virtual error ApplyShaderVariables() override;
        virtual future_error AsyncApplyShaderVariables() override;

    private:
        void LinkShaders();
        void RetrieveShaderVariables();

        void ClearVariableArray();

        void AppendShaderVariable(const std::string& name, const std::string& semantic, GLenum type, int elements);
        void AppendShaderSamplerVariable(const std::string& name, const std::string& semantic,
            const std::string& samp_name, const std::string& samp_semantic,
            unsigned int& slot_index, GLenum type, int elements);

    private:
        GLuint m_program;
        typedef std::vector<Graphics::IShaderVariablePtr> VariableArray;
        VariableArray m_variableArray;
        bool m_hasLinked;
    };
}

#endif
#endif // SHADER_PROGRAM_EGL_H
