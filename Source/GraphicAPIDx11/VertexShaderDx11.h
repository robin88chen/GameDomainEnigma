/*********************************************************************
 * \file   VertexShaderDx11.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_SHADER_DX11_H
#define VERTEX_SHADER_DX11_H

#include "GraphicKernel/IVertexShader.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GraphicKernel/IVertexDeclaration.h"
#include "GraphicKernel/VertexDescription.h"
#include <d3d11.h>
#include <D3D11Shader.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class VertexShaderDx11 : public Graphics::IVertexShader
    {
    public:
        VertexShaderDx11(const std::string& name);
        VertexShaderDx11(const VertexShaderDx11&) = delete;
        VertexShaderDx11(VertexShaderDx11&&) = delete;
        virtual ~VertexShaderDx11();
        VertexShaderDx11& operator=(const VertexShaderDx11&) = delete;
        VertexShaderDx11& operator=(VertexShaderDx11&&) = delete;

        ID3D11VertexShader* GetD3DShader() const { return m_d3dShader; };
        ID3D11ShaderReflection* GetD3DShaderReflection() const { return m_d3dShaderReflect; }

        const Graphics::VertexFormatCode& GetShaderVertexFormat() const { return m_shaderVertexFormat; };
        const byte_buffer& GetShaderSignatureBytes() const { return m_shaderByteCode; };

        const Graphics::IShaderVariable::SemanticNameTable& GetSemanticTable() const { return m_semanticTable; }
    protected:
        virtual error CompileCode(
            const std::string& code, const std::string& profile, const std::string& entry) override;

    private:
        /** @remark Semantic 寫在 HLSL 的 註解裡, 格式是:
        "//semantic var_name VAR_SEMANTIC"
        一行一個變數。 註解兩撇之後直接接 "semantic"， 後面接變數名稱與Semantic
        例如 :
        "//semantic anim_time ANIM_TIMER"
        "//semantic mxWorld WORLD_TRANSFORM */
        void ParseSemanticTable(const std::string& code);

        void VertexFormatSemanticMapping(const std::string& semantic_name, const unsigned int semantic_index, const unsigned char mask);
        void MakeVertexFormatCode();

    private:
        ID3D11VertexShader* m_d3dShader;
        ID3D11ShaderReflection* m_d3dShaderReflect;

        Graphics::VertexFormatCode m_shaderVertexFormat;
        byte_buffer m_shaderByteCode;

        Graphics::IShaderVariable::SemanticNameTable m_semanticTable;
    };
}


#endif // VERTEX_SHADER_DX11_H
