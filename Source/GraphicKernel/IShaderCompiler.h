/*********************************************************************
 * \file   IShaderCompiler.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SHADER_COMPILER_INTERFACE_H
#define SHADER_COMPILER_INTERFACE_H

#include <string>
#include <system_error>
#include <memory>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IVertexShader;
    using IVertexShaderPtr = std::shared_ptr<IVertexShader>;
    class IPixelShader;
    using IPixelShaderPtr = std::shared_ptr<IPixelShader>;

    class IShaderCompiler
    {
    public:
        IShaderCompiler();
        IShaderCompiler(const IShaderCompiler&) = delete;
        IShaderCompiler(IShaderCompiler&&) = delete;
        virtual ~IShaderCompiler();
        IShaderCompiler& operator=(const IShaderCompiler&) = delete;
        IShaderCompiler& operator=(IShaderCompiler&&) = delete;

        /** compile vertex shader
        @param code HLSL/GLSL shader code
        @param profile "vs_1_1", "vs_2_0" ....
        @param entry entry point function name
        */
        virtual error CompileVertexShader(const IVertexShaderPtr& shader, 
            const std::string& code, const std::string& profile, const std::string& entry) = 0;

        /** compile pixel shader
        @param code HLSL/GLSL shader code
        @param profile "ps_1_1", "ps_2_0" ....
        @param entry entry point function name
        */
        virtual error CompilePixelShader(const IPixelShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) = 0;
    };
}


#endif // SHADER_COMPILER_INTERFACE_H
