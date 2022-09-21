#include "VertexShaderDx11.h"
#include "GraphicAPIDx11.h"
#include "VertexDeclarationDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/IVertexDeclaration.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/StringFormat.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <d3dcompiler.h>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

VertexShaderDx11::VertexShaderDx11(const std::string& name) : IVertexShader(name)
{
    m_d3dShader = nullptr;
    m_d3dShaderReflect = nullptr;
    m_semanticTable.clear();
}

VertexShaderDx11::~VertexShaderDx11()
{
    SAFE_RELEASE(m_d3dShader);
    SAFE_RELEASE(m_d3dShaderReflect);
    m_semanticTable.clear();
}

void VertexShaderDx11::VertexFormatSemanticMapping(const std::string& semantic_name, const unsigned semantic_index,
    const unsigned char mask)
{
    if (semantic_name.empty()) return;
    if (semantic_name == VertexDeclarationDx11::m_positionSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode =
            (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                | Graphics::IVertexDeclaration::VertexFormatCode::XYZ;
    }
    else if (semantic_name == VertexDeclarationDx11::m_weightsSemanticName)
    {
        switch (mask)
        {
        case 0x01:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB1;
            break;
        case 0x03:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB2;
            break;
        case 0x07:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB3;
            break;
        case 0x0f:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB4;
            break;
        }
    }
    else if (semantic_name == VertexDeclarationDx11::m_boneIndexSemanticName)
    {
        unsigned int pos_code = m_shaderVertexFormat.m_fvfCode & Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK;
        switch (pos_code)
        {
        case Graphics::IVertexDeclaration::VertexFormatCode::XYZ:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB1;
            break;
        case Graphics::IVertexDeclaration::VertexFormatCode::XYZB1:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB2;
            break;
        case Graphics::IVertexDeclaration::VertexFormatCode::XYZB2:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB3;
            break;
        case Graphics::IVertexDeclaration::VertexFormatCode::XYZB3:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB4;
            break;
        case Graphics::IVertexDeclaration::VertexFormatCode::XYZB4:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::IVertexDeclaration::VertexFormatCode::POSITION_MASK))
                    | Graphics::IVertexDeclaration::VertexFormatCode::XYZB5;
            break;
        }
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::LASTBETA_UBYTE4;
    }
    else if (semantic_name == VertexDeclarationDx11::m_normalSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::NORMAL;
    }
    else if (semantic_name == VertexDeclarationDx11::m_tangentSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::TANGENT;
    }
    else if (semantic_name == VertexDeclarationDx11::m_binormalSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::BINORMAL;
    }
    else if (semantic_name == VertexDeclarationDx11::m_color0SemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::F_DIFFUSE;
    }
    else if (semantic_name == VertexDeclarationDx11::m_color1SemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::IVertexDeclaration::VertexFormatCode::F_SPECULAR;
    }
    else if (semantic_name == VertexDeclarationDx11::m_texCoordSemanticName)
    {
        if (semantic_index >= Graphics::IVertexDeclaration::VertexFormatCode::MAX_TEX_COORD) return;
        if (m_shaderVertexFormat.m_texCount <= semantic_index) m_shaderVertexFormat.m_texCount = semantic_index + 1;
        if (mask == 0x01)
        {
            m_shaderVertexFormat.m_texCoordSize[semantic_index] = 1;
        }
        else if (mask == 0x03)
        {
            m_shaderVertexFormat.m_texCoordSize[semantic_index] = 2;
        }
        else if (mask == 0x07)
        {
            m_shaderVertexFormat.m_texCoordSize[semantic_index] = 3;
        }
        else if (mask == 0x0f)
        {
            m_shaderVertexFormat.m_texCoordSize[semantic_index] = 4;
        }
    }
}

void VertexShaderDx11::MakeVertexFormatCode()
{
    if (FATAL_LOG_EXPR(!m_d3dShaderReflect)) return;
    D3D11_SHADER_DESC shader_desc;
    m_d3dShaderReflect->GetDesc(&shader_desc);
    if (FATAL_LOG_EXPR(shader_desc.InputParameters == 0)) return;

    for (unsigned int i = 0; i < shader_desc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        m_d3dShaderReflect->GetInputParameterDesc(i, &param_desc);
        VertexFormatSemanticMapping(param_desc.SemanticName, param_desc.SemanticIndex, param_desc.Mask);
    }
}


void VertexShaderDx11::ParseSemanticTable(const std::string& code)
{
    m_semanticTable.clear();
#include "ParseSemanticTable.inl"
}
