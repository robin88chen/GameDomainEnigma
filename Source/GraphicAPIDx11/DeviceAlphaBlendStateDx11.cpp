#include "DeviceAlphaBlendStateDx11.h"
#include "GraphicAPIDx11.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceAlphaBlendStateDx11::DeviceAlphaBlendStateDx11(const std::string& name) : IDeviceAlphaBlendState(name)
{
    m_d3dBlend = nullptr;
}

DeviceAlphaBlendStateDx11::~DeviceAlphaBlendStateDx11()
{
    SAFE_RELEASE(m_d3dBlend);
}

error DeviceAlphaBlendStateDx11::CreateFromData(const BlendStateData& data)
{
    IDeviceAlphaBlendState::CreateFromData(data);

    SAFE_RELEASE(m_d3dBlend);

    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_BLEND_DESC desc;
    desc.AlphaToCoverageEnable = FALSE;
    if (!m_data.m_isMultiBlendTarget)
    {
        desc.IndependentBlendEnable = FALSE;
        FillBlendOps(desc.RenderTarget[0], m_data.m_blendType[0]);
    }
    else
    {
        desc.IndependentBlendEnable = TRUE;
        for (int i = 0; i < MAX_BLEND_TARGET; i++)
        {
            FillBlendOps(desc.RenderTarget[i], m_data.m_blendType[i]);
        }
    }
    HRESULT hr = device->CreateBlendState(&desc, &m_d3dBlend);
    if (FATAL_LOG_EXPR(hr)) return ErrorCode::dxCreateBlendState;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::AlphaBlendStateResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceAlphaBlendStateDx11::BindToDevice()
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    deviceContext->OMSetBlendState(m_d3dBlend, nullptr, 0xffffffff);

    return ErrorCode::ok;
}

void DeviceAlphaBlendStateDx11::FillBlendOps(D3D11_RENDER_TARGET_BLEND_DESC& desc, BlendStateData::BlendType type)
{
    switch (type)
    {
    case BlendStateData::BlendType::Blend_Disable:
    {
        desc.BlendEnable = FALSE;
        desc.SrcBlend = D3D11_BLEND_ONE;
        desc.DestBlend = D3D11_BLEND_ZERO;
        desc.BlendOp = D3D11_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    break;
    case BlendStateData::BlendType::Blend_Opaque:
    {
        desc.BlendEnable = TRUE;
        desc.SrcBlend = D3D11_BLEND_ONE;
        desc.DestBlend = D3D11_BLEND_ZERO;
        desc.BlendOp = D3D11_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    break;
    case BlendStateData::BlendType::Blend_Additive:
    {
        desc.BlendEnable = TRUE;
        desc.SrcBlend = D3D11_BLEND_ONE;
        desc.DestBlend = D3D11_BLEND_ONE;
        desc.BlendOp = D3D11_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    break;
    case BlendStateData::BlendType::Blend_Transparent:
    {
        desc.BlendEnable = TRUE;
        desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D11_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    break;
    }
}
