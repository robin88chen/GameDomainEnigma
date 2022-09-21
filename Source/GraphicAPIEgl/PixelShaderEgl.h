/*********************************************************************
 * \file   PixelShaderEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef PIXEL_SHADER_EGL_H
#define PIXEL_SHADER_EGL_H

#include "GraphicKernel/IPixelShader.h"
#include "GraphicKernel/IShaderVariable.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class PixelShaderEgl : public Graphics::IPixelShader
    {
    public:
        PixelShaderEgl(const std::string& name);
        PixelShaderEgl(const PixelShaderEgl&) = delete;
        PixelShaderEgl(PixelShaderEgl&&) = delete;
        virtual ~PixelShaderEgl();
        PixelShaderEgl& operator=(const PixelShaderEgl&) = delete;
        PixelShaderEgl& operator=(PixelShaderEgl&&) = delete;

        GLuint GetShader() const { return m_shader; };

        std::string GetVarSemantic(const std::string& var_name);
        std::string GetSamplerStateName(const std::string& tex_var_name);

    private:
        /** @remark Semantic 寫在 HLSL 的 註解裡, 格式是:
        "//semantic var_name VAR_SEMANTIC"
        一行一個變數。 註解兩撇之後直接接 "semantic"， 後面接變數名稱與Semantic
        例如 :
        "//semantic anim_time ANIM_TIMER"
        "//semantic mxWorld WORLD_TRANSFORM */
        void ParseSemanticTable(const std::string& code);

        /** 自訂的 SamplerState 與 Texture 變數對應表
        @remark Sampler State 寫在註解裡, 格式:
        一行一個變數。 註解兩撇之後直接接 "sampler_state"， 後面接變數名稱與對應的 texture 變數名稱
        例如 :
        "//sampler_state samLinear DiffseTexture
        */
        void ParseSamplerStateTable(const std::string& code);
    private:
        friend class ShaderCompilerEgl;
        GLuint m_shader;
        Graphics::IShaderVariable::SemanticNameTable m_varSemanticTable;
        Graphics::IShaderVariable::SemanticNameTable m_texSamplerTable;
    };
}

#endif
#endif // PIXEL_SHADER_EGL_H
