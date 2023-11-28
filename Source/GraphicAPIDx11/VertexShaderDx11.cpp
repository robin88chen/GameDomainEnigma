#include "VertexShaderDx11.h"
#include "GraphicAPIDx11.h"
#include "VertexDeclarationDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/StringFormat.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <d3dcompiler.h>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

// 這個 IID 是新版的, 正確的 GUID 在 dxguid.lib 裡, d3dcompiler.lib 裡是舊的,
// 但為了避免錯誤, 還是重新定義一個
//DEFINE_GUID(Fix_IID_ID3D11ShaderReflection, 0x8d536ca1, 0x0cca, 0x4956, 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84);
EXTERN_C const GUID DECLSPEC_SELECTANY Fix_IID_ID3D11ShaderReflection
= { 0x8d536ca1, 0x0cca, 0x4956, { 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84 } };

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

error VertexShaderDx11::CompileCode(const std::string& code, const std::string& profile, const std::string& entry)
{
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    size_t code_leng = code.length();
    ID3DBlob* outBuf = nullptr;
    ID3DBlob* errorBuf = nullptr;
    DWORD flag1 = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
#ifdef _DEBUG
    flag1 |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

    HRESULT hr = D3DCompile(code.c_str(), code_leng, nullptr, nullptr, nullptr,
        entry.c_str(), profile.c_str(), flag1, 0, &outBuf, &errorBuf);
    if (FAILED(hr))
    {
        std::string str = string_format("%s Shader Compile Failed : %s", m_name.c_str(), errorBuf->GetBufferPointer());
        LOG(Error, str);
        SAFE_RELEASE(outBuf);
        SAFE_RELEASE(errorBuf);
        Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexShaderCompileFailed>(m_name, str));
        return ErrorCode::compileShader;
    }
    SAFE_RELEASE(errorBuf);
    m_shaderByteCode = make_data_buffer(static_cast<unsigned char*>(outBuf->GetBufferPointer()), outBuf->GetBufferSize());

    hr = graphic->GetD3DDevice()->CreateVertexShader(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), nullptr, &m_d3dShader);
    //GUID guid = IID_ID3D11ShaderReflection;

    //D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), Fix_IID_ID3D11ShaderReflection,
      //  reinterpret_cast<void**>(&m_d3dShaderReflect));
    D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), IID_ID3D11ShaderReflection,
        reinterpret_cast<void**>(&m_d3dShaderReflect));
    SAFE_RELEASE(outBuf);

    ParseSemanticTable(code);
    //RetrieveShaderVariables(graphic->GetD3DDevice(), semantic_table, IShaderVariable::VarOfVertexShader);
    //if (semantic_table) delete semantic_table;

    MakeVertexFormatCode();

    if (FAILED(hr)) return ErrorCode::dxCreateShader;

    m_hasCompiled = true;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexShaderCompiled>(m_name));
    return ErrorCode::ok;
}

void VertexShaderDx11::VertexFormatSemanticMapping(const std::string& semantic_name, const unsigned semantic_index,
    const unsigned char mask)
{
    if (semantic_name.empty()) return;
    if (semantic_name == VertexDeclarationDx11::m_positionSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode =
            (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                | Graphics::VertexFormatCode::XYZ;
    }
    else if (semantic_name == VertexDeclarationDx11::m_weightsSemanticName)
    {
        switch (mask)
        {
        case 0x01:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB1;
            break;
        case 0x03:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB2;
            break;
        case 0x07:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB3;
            break;
        case 0x0f:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB4;
            break;
            default:
                break;
        }
    }
    else if (semantic_name == VertexDeclarationDx11::m_boneIndexSemanticName)
    {
        unsigned int pos_code = m_shaderVertexFormat.m_fvfCode & Graphics::VertexFormatCode::POSITION_MASK;
        switch (pos_code)
        {
        case Graphics::VertexFormatCode::XYZ:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB1;
            break;
        case Graphics::VertexFormatCode::XYZB1:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB2;
            break;
        case Graphics::VertexFormatCode::XYZB2:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB3;
            break;
        case Graphics::VertexFormatCode::XYZB3:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB4;
            break;
        case Graphics::VertexFormatCode::XYZB4:
            m_shaderVertexFormat.m_fvfCode =
                (m_shaderVertexFormat.m_fvfCode & (~Graphics::VertexFormatCode::POSITION_MASK))
                    | Graphics::VertexFormatCode::XYZB5;
            break;
            default:
                break;
        }
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::LASTBETA_UBYTE4;
    }
    else if (semantic_name == VertexDeclarationDx11::m_normalSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    }
    else if (semantic_name == VertexDeclarationDx11::m_tangentSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::TANGENT;
    }
    else if (semantic_name == VertexDeclarationDx11::m_binormalSemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::BINORMAL;
    }
    else if (semantic_name == VertexDeclarationDx11::m_color0SemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::F_DIFFUSE;
    }
    else if (semantic_name == VertexDeclarationDx11::m_color1SemanticName)
    {
        m_shaderVertexFormat.m_fvfCode |= Graphics::VertexFormatCode::F_SPECULAR;
    }
    else if (semantic_name == VertexDeclarationDx11::m_texCoordSemanticName)
    {
        if (semantic_index >= Graphics::VertexFormatCode::MAX_TEX_COORD) return;
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
