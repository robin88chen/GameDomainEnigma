/*********************************************************************
 * \file   VertexDeclarationDx11.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_DECLARATION_DX11_H
#define VERTEX_DECLARATION_DX11_H

#include "GraphicKernel/IVertexDeclaration.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <D3D11.h>
#include <system_error>

namespace Enigma::Devices
{
    using error = std::error_code;
    class VertexShaderDx11;
    /** vertex declaration dx11 */
    class VertexDeclarationDx11 : public Graphics::IVertexDeclaration
    {
    public:
        VertexDeclarationDx11(const std::string& name, const std::string& data_vertex_format,
            const Graphics::VertexFormatCode& shader_fmt_code);
        VertexDeclarationDx11(const VertexDeclarationDx11&) = delete;
        VertexDeclarationDx11(VertexDeclarationDx11&&) = delete;
        virtual ~VertexDeclarationDx11() override;
        VertexDeclarationDx11& operator=(const VertexDeclarationDx11&) = delete;
        VertexDeclarationDx11& operator=(VertexDeclarationDx11&&) = delete;

        virtual bool IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader) override;

        error CreateD3DInputLayout(ID3D11Device* device);
        ID3D11InputLayout* GetD3DInputLayout() const { return m_d3dInputLayout; }

        void FillShaderVertexFormat(const std::shared_ptr<VertexShaderDx11>& shader);

    protected:
        std::tuple<D3D11_INPUT_ELEMENT_DESC* , unsigned int> CreateVertexLayout(const Graphics::VertexDescription& vertex_desc);

    public:
        // HLSL Semantic String
        static const std::string m_positionSemanticName;
        static const std::string m_normalSemanticName;
        static const std::string m_color0SemanticName;
        static const std::string m_color1SemanticName;
        static const std::string m_texCoordSemanticName;
        static const std::string m_weightsSemanticName;
        static const std::string m_boneIndexSemanticName;
        static const std::string m_tangentSemanticName;
        static const std::string m_binormalSemanticName;
    protected:
        //VertexFormatCode m_EffectVertexFormatCode;
        byte_buffer m_shaderSignatureBytes;
        ID3D11InputLayout* m_d3dInputLayout;

        Graphics::VertexFormatCode m_shaderVertexFormat;
    };

};


#endif // VERTEX_DECLARATION_DX11_H
