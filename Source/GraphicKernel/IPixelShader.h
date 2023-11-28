/*********************************************************************
 * \file   IPixelShader.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef PIXEL_SHADER_INTERFACE_H
#define PIXEL_SHADER_INTERFACE_H

#include <memory>
#include <string>
#include <system_error>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IPixelShader : public std::enable_shared_from_this<IPixelShader>
    {
    public:
        IPixelShader(const std::string& name);
        IPixelShader(const IPixelShader&) = delete;
        IPixelShader(IPixelShader&&) = delete;
        virtual ~IPixelShader();
        IPixelShader& operator=(const IPixelShader&) = delete;
        IPixelShader& operator=(IPixelShader&&) = delete;

        const std::string& getName() { return m_name; }
        /** compile pixel shader
        @param code HLSL/GLSL shader code
        @param profile "ps_1_1", "ps_2_0" ....
        @param entry entry point function name
        */
        void Compile(const std::string& code, const std::string& profile, const std::string& entry);

        bool HasCompiled() { return m_hasCompiled; }

    protected:
        virtual error CompileCode(const std::string& code, const std::string& profile, const std::string& entry) = 0;

    protected:
        std::string m_name;
        bool m_hasCompiled;
    };
    using IPixelShaderPtr = std::shared_ptr<IPixelShader>;
    using IPixelShaderWeak = std::weak_ptr<IPixelShader>;
}

#endif // PIXEL_SHADER_INTERFACE_H
