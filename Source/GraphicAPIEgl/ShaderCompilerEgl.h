/*********************************************************************
 * \file   ShaderCompilerEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SHADER_COMPILER_EGL_H
#define SHADER_COMPILER_EGL_H

#include "GraphicKernel/IShaderCompiler.h"
#include <string>
#include <system_error>

#if TARGET_PLATFORM == PLATFORM_ANDROID

namespace Enigma::Devices
{
    using error = std::error_code;

    class ShaderCompilerEgl : public Graphics::IShaderCompiler
    {
    public:
        ShaderCompilerEgl();
        ~ShaderCompilerEgl();

        virtual error CompileVertexShader(const Graphics::IVertexShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) override;
        virtual error CompilePixelShader(const Graphics::IPixelShaderPtr& shader,
            const std::string& code, const std::string& profile, const std::string& entry) override;
    };
}

#endif
#endif // SHADER_COMPILER_EGL_H
