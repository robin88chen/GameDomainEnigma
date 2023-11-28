#include "DepthStencilSurfaceDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode  = Enigma::Graphics::ErrorCode;

extern DXGI_FORMAT ConvertGraphicFormatToDXGI(const Enigma::Graphics::GraphicFormat& format);
extern unsigned int ConvertDXGIFormatToGraphicFormat(DXGI_FORMAT fmt);

DepthStencilSurfaceDx11::DepthStencilSurfaceDx11(const std::string& name, ID3D11Device* device,
    const MathLib::Dimension<unsigned>& dimension, const Graphics::GraphicFormat& fmt) : IDepthStencilSurface(name)
{
    assert(device);
    m_d3dSurface = nullptr;
    m_d3dDepthView = nullptr;
    m_dimension = MathLib::Dimension<unsigned>{ 0, 0 };
    Create(device, dimension, fmt);
    if (m_d3dSurface) CreateD3DDepthView(device);
}

DepthStencilSurfaceDx11::DepthStencilSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D* buff)
    : IDepthStencilSurface(name)
{
    m_d3dSurface = buff;
    m_d3dDepthView = nullptr;
    m_dimension = MathLib::Dimension<unsigned>{ 0, 0 };
    if (m_d3dSurface)
    {
        D3D11_TEXTURE2D_DESC desc;
        m_d3dSurface->GetDesc(&desc);
        m_dimension.m_height = desc.Height;
        m_dimension.m_width = desc.Width;
        m_format.fmt = ConvertDXGIFormatToGraphicFormat(desc.Format);
    }
    if (m_d3dSurface) CreateD3DDepthView(device);
}

DepthStencilSurfaceDx11::DepthStencilSurfaceDx11(const std::string& name, const std::shared_ptr<DepthStencilSurfaceDx11>& depthDx11)
    : IDepthStencilSurface(name)
{
    m_dimension = depthDx11->m_dimension;
    m_format = depthDx11->m_format;
    m_d3dSurface = depthDx11->m_d3dSurface;
    if (m_d3dSurface) m_d3dSurface->AddRef();
    m_d3dDepthView = depthDx11->m_d3dDepthView;
    if (m_d3dDepthView) m_d3dDepthView->AddRef();
}

DepthStencilSurfaceDx11::~DepthStencilSurfaceDx11()
{
    SAFE_RELEASE(m_d3dSurface);
    SAFE_RELEASE(m_d3dDepthView);
}

error DepthStencilSurfaceDx11::Resize(const MathLib::Dimension<unsigned>& dimension)
{
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    SAFE_RELEASE(m_d3dDepthView);
    SAFE_RELEASE(m_d3dSurface);

    error er = Create(graphic->GetD3DDevice(), dimension, m_format);
    if (er) return er;
    CreateD3DDepthView(graphic->GetD3DDevice());
    Frameworks::EventPublisher::post(std::make_shared<Graphics::DepthSurfaceResized>(m_name, m_dimension));

    return er;
}

error DepthStencilSurfaceDx11::Create(ID3D11Device* device, const MathLib::Dimension<unsigned>& dimension,
    const Graphics::GraphicFormat& fmt)
{
    assert(device);
    if (m_d3dSurface) return ErrorCode::ok;

    D3D11_TEXTURE2D_DESC descDepth;

    descDepth.Width = dimension.m_width;
    descDepth.Height = dimension.m_height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = ConvertGraphicFormatToDXGI(fmt);
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT hr = device->CreateTexture2D(&descDepth, NULL, &m_d3dSurface);
    if (FAILED(hr)) return ErrorCode::createDepthSurfaceFail;

    m_dimension = dimension;
    m_format = fmt;

    return ErrorCode::ok;
}

void DepthStencilSurfaceDx11::CreateD3DDepthView(ID3D11Device* device)
{
    assert(device);
    assert(m_d3dSurface);

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    descDSV.Format = ConvertGraphicFormatToDXGI(m_format);
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    HRESULT hr = device->CreateDepthStencilView(m_d3dSurface, &descDSV, &m_d3dDepthView);
    if (FATAL_LOG_EXPR(FAILED(hr))) return;
}
