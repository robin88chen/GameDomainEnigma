#include "DeviceDepthStencilStateDx11.h"
#include "GraphicAPIDx11.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceDepthStencilStateDx11::DeviceDepthStencilStateDx11(const std::string& name) : IDeviceDepthStencilState(name)
{
    m_d3dDepthStencil = nullptr;
}

DeviceDepthStencilStateDx11::~DeviceDepthStencilStateDx11()
{
    SAFE_RELEASE(m_d3dDepthStencil);
}

error DeviceDepthStencilStateDx11::CreateFromData(const DepthStencilData& data)
{
    IDeviceDepthStencilState::CreateFromData(data);

    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_DEPTH_STENCIL_DESC desc;
    if (m_data.m_isDepthTestEnable)
    {
        desc.DepthEnable = TRUE;
    }
    else
    {
        desc.DepthEnable = FALSE;
    }
    if (m_data.m_isDepthWriteEnable)
    {
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    }
    else
    {
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    }
    if (m_data.m_isStencilTestEnable)
    {
        desc.StencilEnable = TRUE;
    }
    else
    {
        desc.StencilEnable = FALSE;
    }
    desc.StencilReadMask = m_data.m_stencilReadMask;
    desc.StencilWriteMask = m_data.m_stencilWriteMask;
    desc.DepthFunc = CompareFuncByData(m_data.m_depthCompare);
    FillStencilOpData(desc.FrontFace, m_data.m_frontFaceOp);
    FillStencilOpData(desc.BackFace, m_data.m_backFaceOp);

    HRESULT hr = device->CreateDepthStencilState(&desc, &m_d3dDepthStencil);
    if (FATAL_LOG_EXPR(hr)) return ErrorCode::dxCreateDepthState;

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::DepthStencilStateResourceCreated(m_name) });
    return ErrorCode::ok;
}

error DeviceDepthStencilStateDx11::BindToDevice()
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    deviceContext->OMSetDepthStencilState(m_d3dDepthStencil, m_data.m_stencilRefValue);

    return ErrorCode::ok;
}

D3D11_COMPARISON_FUNC DeviceDepthStencilStateDx11::CompareFuncByData(DepthStencilData::CompareFunc mode)
{
    switch (mode)
    {
    case DepthStencilData::CompareFunc::Never:
        return D3D11_COMPARISON_NEVER;
        break;
    case DepthStencilData::CompareFunc::Less:
        return D3D11_COMPARISON_LESS;
        break;
    case DepthStencilData::CompareFunc::Equal:
        return D3D11_COMPARISON_EQUAL;
        break;
    case DepthStencilData::CompareFunc::LessEqual:
        return D3D11_COMPARISON_LESS_EQUAL;
        break;
    case DepthStencilData::CompareFunc::Greater:
        return D3D11_COMPARISON_GREATER;
        break;
    case DepthStencilData::CompareFunc::NotEqual:
        return D3D11_COMPARISON_NOT_EQUAL;
        break;
    case DepthStencilData::CompareFunc::GreaterEqual:
        return D3D11_COMPARISON_GREATER_EQUAL;
        break;
    case DepthStencilData::CompareFunc::Always:
        return D3D11_COMPARISON_ALWAYS;
        break;
    }
    return D3D11_COMPARISON_NEVER;
}

void DeviceDepthStencilStateDx11::FillStencilOpData(D3D11_DEPTH_STENCILOP_DESC& desc,
    const DepthStencilData::StencilOpData& data)
{
    desc.StencilFunc = CompareFuncByData(data.m_compare);
    desc.StencilDepthFailOp = OpCodeByData(data.m_depthFailOp);
    desc.StencilFailOp = OpCodeByData(data.m_failOp);
    desc.StencilPassOp = OpCodeByData(data.m_passOp);
}

D3D11_STENCIL_OP DeviceDepthStencilStateDx11::OpCodeByData(DepthStencilData::StencilOpCode code)
{
    switch (code)
    {
    case DepthStencilData::StencilOpCode::Keep:
        return D3D11_STENCIL_OP_KEEP;
        break;
    case DepthStencilData::StencilOpCode::Zero:
        return D3D11_STENCIL_OP_ZERO;
        break;
    case DepthStencilData::StencilOpCode::Replace:
        return D3D11_STENCIL_OP_REPLACE;
        break;
    case DepthStencilData::StencilOpCode::Incr_Sat:
        return D3D11_STENCIL_OP_INCR_SAT;
        break;
    case DepthStencilData::StencilOpCode::Decr_Sat:
        return D3D11_STENCIL_OP_DECR_SAT;
        break;
    case DepthStencilData::StencilOpCode::Invert:
        return D3D11_STENCIL_OP_INVERT;
        break;
    case DepthStencilData::StencilOpCode::Incr:
        return D3D11_STENCIL_OP_INCR;
        break;
    case DepthStencilData::StencilOpCode::Decr:
        return D3D11_STENCIL_OP_DECR;
        break;
    }
    return D3D11_STENCIL_OP_KEEP;
}
