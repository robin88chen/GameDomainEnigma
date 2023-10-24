#include "DeviceRasterizerStateDx11.h"
#include "GraphicAPIDx11.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceRasterizerStateDx11::DeviceRasterizerStateDx11(const std::string& name) : IDeviceRasterizerState(name)
{
    m_d3dRasterizer = nullptr;
}

DeviceRasterizerStateDx11::~DeviceRasterizerStateDx11()
{
    SAFE_RELEASE(m_d3dRasterizer);
}

error DeviceRasterizerStateDx11::CreateFromData(const RasterizerStateData& data)
{
    IDeviceRasterizerState::CreateFromData(data);

    SAFE_RELEASE(m_d3dRasterizer);

    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_RASTERIZER_DESC desc;
    desc.FillMode = FillModeByData(m_data.m_fillMode);
    desc.CullMode = CullModeByData(m_data.m_cullMode);
    desc.FrontCounterClockwise = BackfaceWiseByData(m_data.m_cullMode);
    desc.DepthBias = PixeledDepthBiasByData(m_data.m_depthBias);
    desc.DepthBiasClamp = m_data.m_depthBiasClamp;
    desc.SlopeScaledDepthBias = m_data.m_slopeScaledDepthBias;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;

    HRESULT hr = device->CreateRasterizerState(&desc, &m_d3dRasterizer);
    if (FATAL_LOG_EXPR(hr)) return ErrorCode::dxCreateRasterizerState;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::RasterizerStateResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceRasterizerStateDx11::BindToDevice()
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    deviceContext->RSSetState(m_d3dRasterizer);

    return ErrorCode::ok;
}

D3D11_FILL_MODE DeviceRasterizerStateDx11::FillModeByData(RasterizerStateData::FillMode mode)
{
    switch (mode)
    {
    case RasterizerStateData::FillMode::Fill:
        return D3D11_FILL_SOLID;
    case RasterizerStateData::FillMode::Wireframe:
        return D3D11_FILL_WIREFRAME;
    case RasterizerStateData::FillMode::Point:
        return D3D11_FILL_SOLID;
    }
    return D3D11_FILL_SOLID;
}

D3D11_CULL_MODE DeviceRasterizerStateDx11::CullModeByData(RasterizerStateData::BackfaceCullMode mode)
{
    switch (mode)
    {
    case RasterizerStateData::BackfaceCullMode::Cull_None:
        return D3D11_CULL_NONE;
    case RasterizerStateData::BackfaceCullMode::Cull_CCW:
        return D3D11_CULL_BACK;  // always cull backface
    case RasterizerStateData::BackfaceCullMode::Cull_CW:
        return D3D11_CULL_BACK; // always cull backface
    }
    return D3D11_CULL_BACK;
}

BOOL DeviceRasterizerStateDx11::BackfaceWiseByData(RasterizerStateData::BackfaceCullMode mode)
{
    switch (mode)
    {
    case RasterizerStateData::BackfaceCullMode::Cull_None:
        return FALSE;
    case RasterizerStateData::BackfaceCullMode::Cull_CCW:
        return FALSE;
    case RasterizerStateData::BackfaceCullMode::Cull_CW:
        return TRUE;
    }
    return FALSE;
}

int DeviceRasterizerStateDx11::PixeledDepthBiasByData(float bias)
{
    unsigned bits = Graphics::IGraphicAPI::Instance()->GetDepthSurfaceFormat().DepthBits();
    if ((bits == 16) || (bits == 24)) // D16UNORM, D24UNORM
    {
        return static_cast<int>(bias * static_cast<float>(pow(2, bits)));
    }
    else if (bits == 32) // D32F
    {
        return static_cast<int>(bias / static_cast<float>(pow(2, exp(1.0f) - 23.0f)));
    }
    return 0;
}
