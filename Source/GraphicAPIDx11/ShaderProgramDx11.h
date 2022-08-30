/*********************************************************************
 * \file   ShaderProgramDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_PROGRAM_DX11_H
#define SHADER_PROGRAM_DX11_H

#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IShaderVariable.h"
#include <D3D11Shader.h>

namespace Enigma::Devices
{
    using error = std::error_code;
    class ShaderVariableDx11_Resource;

    class ShaderProgramDx11 : public Graphics::IShaderProgram
    {
    public:
        ShaderProgramDx11(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader, 
            const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl);
        ShaderProgramDx11(const ShaderProgramDx11&) = delete;
        ShaderProgramDx11(ShaderProgramDx11&&) = delete;
        virtual ~ShaderProgramDx11();
        ShaderProgramDx11& operator=(const ShaderProgramDx11&) = delete;
        ShaderProgramDx11& operator=(ShaderProgramDx11&&) = delete;

        virtual Graphics::IShaderVariablePtr GetVariableByName(const std::string& name) override;
        virtual Graphics::IShaderVariablePtr GetVariableBySemantic(const std::string& semantic) override;
        virtual unsigned int GetVariableCount() override;
        virtual Graphics::IShaderVariablePtr GetVariableByIndex(unsigned int index) override;

        //virtual error CommitVariables() override;

        virtual error ApplyVariables() override;
        virtual future_error AsyncApplyVariables() override;
    private:
        void RetrieveShaderVariables(ID3D11ShaderReflection* shaderReflect,
            const Graphics::IShaderVariable::SemanticNameTable& semantic_table,
            Graphics::IShaderVariable::VarOwner var_of);

    private:
        std::vector<std::shared_ptr<ShaderVariableDx11_Resource>> m_variables;
    };
}


#endif // SHADER_PROGRAM_DX11_H
