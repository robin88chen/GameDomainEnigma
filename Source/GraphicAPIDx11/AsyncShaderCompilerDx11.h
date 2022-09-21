/*********************************************************************
 * \file   AsyncShaderCompilerDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef ASYNC_SHADER_COMPILER_DX11_H
#define ASYNC_SHADER_COMPILER_DX11_H

#include "ShaderCompilerDx11.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class AsyncShaderCompilerDx11 : public ShaderCompilerDx11
    {
    public:
        AsyncShaderCompilerDx11(ID3D11Device* device);
        ~AsyncShaderCompilerDx11();

        virtual error CompileVertexShader(const Graphics::IVertexShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) override;
        virtual error CompilePixelShader(const Graphics::IPixelShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) override;
    };
}


#endif // ASYNC_SHADER_COMPILER_DX11_H
