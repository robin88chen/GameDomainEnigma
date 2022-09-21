#include "ShaderCompilerDx11.h"
#include "VertexShaderDx11.h"
#include "PixelShaderDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/StringFormat.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <d3dcompiler.h>

using namespace Enigma::Graphics;
using namespace Enigma::Devices;

// 這個 IID 是新版的, 正確的 GUID 在 dxguid.lib 裡, d3dcompiler.lib 裡是舊的, 
// 但為了避免錯誤, 還是重新定義一個
//DEFINE_GUID(Fix_IID_ID3D11ShaderReflection, 0x8d536ca1, 0x0cca, 0x4956, 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84);
EXTERN_C const GUID DECLSPEC_SELECTANY Fix_IID_ID3D11ShaderReflection
= { 0x8d536ca1, 0x0cca, 0x4956, { 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84 } };

ShaderCompilerDx11::ShaderCompilerDx11(ID3D11Device* device)
{
    m_d3dDevice = device;
}

ShaderCompilerDx11::~ShaderCompilerDx11()
{
    m_d3dDevice = nullptr;
}

error ShaderCompilerDx11::CompileVertexShader(const IVertexShaderPtr& shader, 
    const std::string& code, const std::string& profile, const std::string& entry)
{
    if (FATAL_LOG_EXPR(!m_d3dDevice)) return ErrorCode::d3dDeviceNullPointer;
    auto shader_dx11 = std::dynamic_pointer_cast<VertexShaderDx11, IVertexShader>(shader);

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
        std::string str = string_format("%s Shader Compile Failed : %s", shader_dx11->m_name.c_str(), errorBuf->GetBufferPointer());
        LOG(Error, str);
        SAFE_RELEASE(outBuf);
        SAFE_RELEASE(errorBuf);
        Frameworks::EventPublisher::Post(std::make_shared<Graphics::VertexShaderCompileFailed>(shader_dx11->m_name, str));
        return ErrorCode::compileShader;
    }
    SAFE_RELEASE(errorBuf);
    shader_dx11->m_shaderByteCode = make_data_buffer((unsigned char*)outBuf->GetBufferPointer(), outBuf->GetBufferSize());

    hr = m_d3dDevice->CreateVertexShader(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), nullptr, &shader_dx11->m_d3dShader);
    GUID guid = IID_ID3D11ShaderReflection;

    D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), Fix_IID_ID3D11ShaderReflection,
        (void**)&shader_dx11->m_d3dShaderReflect);
    SAFE_RELEASE(outBuf);

    shader_dx11->ParseSemanticTable(code);
    //RetrieveShaderVariables(graphic->GetD3DDevice(), semantic_table, IShaderVariable::VarOfVertexShader);
    //if (semantic_table) delete semantic_table;

    shader_dx11->MakeVertexFormatCode();

    if (FAILED(hr)) return ErrorCode::dxCreateShader;

    shader_dx11->m_hasCompiled = true;

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::VertexShaderCompiled>(shader_dx11->m_name));
    return ErrorCode::ok;
}

error ShaderCompilerDx11::CompilePixelShader(const IPixelShaderPtr& shader, 
    const std::string& code, const std::string& profile, const std::string& entry)
{
    if (FATAL_LOG_EXPR(!m_d3dDevice)) return ErrorCode::d3dDeviceNullPointer;
    auto shader_dx11 = std::dynamic_pointer_cast<PixelShaderDx11, IPixelShader>(shader);

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
        std::string str = string_format("%s Shader Compile Failed : %s", shader_dx11->m_name.c_str(), errorBuf->GetBufferPointer());
        LOG(Error, str);
        SAFE_RELEASE(outBuf);
        SAFE_RELEASE(errorBuf);
        Frameworks::EventPublisher::Post(std::make_shared<Graphics::PixelShaderCompileFailed>(shader_dx11->m_name, str));
        return ErrorCode::compileShader;
    }
    SAFE_RELEASE(errorBuf);
    hr = m_d3dDevice->CreatePixelShader(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), NULL, &shader_dx11->m_d3dShader);
    GUID guid = IID_ID3D11ShaderReflection;
    D3DReflect(outBuf->GetBufferPointer(), outBuf->GetBufferSize(), Fix_IID_ID3D11ShaderReflection,
        (void**)&shader_dx11->m_d3dShaderReflect);
    SAFE_RELEASE(outBuf);

    shader_dx11->ParseSemanticTable(code);
    //RetrieveShaderVariables(graphic->GetD3DDevice(), semantic_table, IShaderVariable::VarOfPixelShader);
    //if (semantic_table) delete semantic_table;

    if (FAILED(hr)) return ErrorCode::dxCreateShader;

    shader_dx11->m_hasCompiled = true;

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::PixelShaderCompiled>(shader_dx11->m_name));
    return ErrorCode::ok;
}
