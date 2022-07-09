/*********************************************************************
 * \file   IShaderProgram.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_PROGRAM_INTERFACE_H
#define SHADER_PROGRAM_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <memory>
#include <future>


namespace Enigma::Graphics
{
    using error = std::error_code;

    class IShaderVariable;
    using IShaderVariablePtr = std::shared_ptr<IShaderVariable>;
    class IVertexShader;
    using IVertexShaderPtr = std::shared_ptr<IVertexShader>;
    class IPixelShader;
    using IPixelShaderPtr = std::shared_ptr<IPixelShader>;
    class IShaderProgram : public std::enable_shared_from_this<IShaderProgram>
    {
    public:
        IShaderProgram(const std::string& name, const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader);
        IShaderProgram(const IShaderProgram&) = delete;
        IShaderProgram(IShaderProgram&&) = delete;
        virtual ~IShaderProgram();
        IShaderProgram& operator=(const IShaderProgram&) = delete;
        IShaderProgram& operator=(IShaderProgram&&) = delete;

        const std::string& GetName() { return m_name; }
        const IVertexShaderPtr& GetVertexShader() { return m_vtxShader; }
        const IPixelShaderPtr& GetPixelShader() { return m_pixShader; }

        virtual IShaderVariablePtr GetVariableByName(const std::string& name) = 0;
        virtual IShaderVariablePtr GetVariableBySemantic(const std::string& semantic) = 0;
        virtual unsigned int GetVariableCount() = 0;
        virtual IShaderVariablePtr GetVariableByIndex(unsigned int index) = 0;

        virtual error CommitVariables() = 0;

        virtual error ApplyVariables() = 0;
        virtual future_error AsyncApplyVariables() = 0;

    protected:
        std::string m_name;
        IVertexShaderPtr m_vtxShader;
        IPixelShaderPtr m_pixShader;
    };
    using IShaderProgramPtr = std::shared_ptr<IShaderProgram>;
    using IShaderProgramWeak = std::weak_ptr<IShaderProgram>;
}

#endif // SHADER_PROGRAM_INTERFACE_H
