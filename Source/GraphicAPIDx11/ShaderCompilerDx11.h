/*********************************************************************
 * \file   ShaderCompilerDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SHADER_COMPILER_DX11_H
#define SHADER_COMPILER_DX11_H

#include "GraphicKernel/IShaderCompiler.h"
#include "d3d11.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class ShaderCompilerDx11 : public Graphics::IShaderCompiler
    {
    public:
        ShaderCompilerDx11(ID3D11Device* device);
        ~ShaderCompilerDx11();

        virtual error CompileVertexShader(const Graphics::IVertexShaderPtr& shader, 
            const std::string& code, const std::string& profile, const std::string& entry) override;
        virtual error CompilePixelShader(const Graphics::IPixelShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) override;

    protected:
        ID3D11Device* m_d3dDevice;
    };
}

#endif // SHADER_COMPILER_DX11_H
