/*********************************************************************
 * \file   PixelShaderDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef PIXEL_SHADER_DX11_H
#define PIXEL_SHADER_DX11_H

#include "GraphicKernel/IPixelShader.h"
#include "GraphicKernel/IShaderVariable.h"
#include <d3d11.h>
#include <D3D11Shader.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class PixelShaderDx11 : public Graphics::IPixelShader
    {
    public:
        PixelShaderDx11(const std::string& name);
        PixelShaderDx11(const PixelShaderDx11&) = delete;
        PixelShaderDx11(PixelShaderDx11&&) = delete;
        virtual ~PixelShaderDx11() override;
        PixelShaderDx11& operator=(const PixelShaderDx11&) = delete;
        PixelShaderDx11& operator=(PixelShaderDx11&&) = delete;

        ID3D11PixelShader* GetD3DShader() const { return m_d3dShader; }
        ID3D11ShaderReflection* GetD3DShaderReflection() const { return m_d3dShaderReflect; }

        const Graphics::IShaderVariable::SemanticNameTable& GetSemanticTable() const { return m_semanticTable; }

    protected:
        virtual error CompileCode(const std::string& code, const std::string& profile, const std::string& entry) override;

    private:
        /** @remark Semantic 寫在 HLSL 的 註解裡, 格式是:
        "//semantic var_name VAR_SEMANTIC"
        一行一個變數。 註解兩撇之後直接接 "semantic"， 後面接變數名稱與Semantic
        例如 :
        "//semantic anim_time ANIM_TIMER"
        "//semantic mxWorld WORLD_TRANSFORM */
        void ParseSemanticTable(const std::string& code);

    private:
        ID3D11PixelShader* m_d3dShader;
        ID3D11ShaderReflection* m_d3dShaderReflect;
        Graphics::IShaderVariable::SemanticNameTable m_semanticTable;
    };
}

#endif // PIXEL_SHADER_DX11_H
