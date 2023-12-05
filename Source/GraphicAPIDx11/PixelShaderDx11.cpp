#include "PixelShaderDx11.h"
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

PixelShaderDx11::PixelShaderDx11(const std::string& name) : IPixelShader(name)
{
    m_d3dShader = nullptr;
    m_d3dShaderReflect = nullptr;
    m_semanticTable.clear();
}

PixelShaderDx11::~PixelShaderDx11()
{
    SAFE_RELEASE(m_d3dShader);
    SAFE_RELEASE(m_d3dShaderReflect);
    m_semanticTable.clear();
}

error PixelShaderDx11::CompileCode(const std::string& code, const std::string& profile, const std::string& entry)
{
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
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
        Frameworks::EventPublisher::post(std::make_shared<Graphics::PixelShaderCompileFailed>(m_name, str));
        return ErrorCode::compileShader;
    }
    SAFE_RELEASE(errorBuf);
    hr = graphic->GetD3DDevice()->CreatePixelShader(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), NULL, &m_d3dShader);
    //GUID guid = IID_ID3D11ShaderReflection;
    //D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), Fix_IID_ID3D11ShaderReflection,
      //  reinterpret_cast<void**>(&m_d3dShaderReflect));
    D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), IID_ID3D11ShaderReflection,
        reinterpret_cast<void**>(&m_d3dShaderReflect));
    SAFE_RELEASE(outBuf);

    ParseSemanticTable(code);
    //RetrieveShaderVariables(graphic->GetD3DDevice(), semantic_table, IShaderVariable::VarOfPixelShader);
    //if (semantic_table) delete semantic_table;

    if (FAILED(hr)) return ErrorCode::dxCreateShader;

    m_hasCompiled = true;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::PixelShaderCompiled>(m_name));
    return ErrorCode::ok;
}

void PixelShaderDx11::ParseSemanticTable(const std::string& code)
{
    m_semanticTable.clear();
#include "ParseSemanticTable.inl"
}
