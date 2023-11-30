#include "BackSurfaceDx11.h"
#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "Platforms/MemoryMacro.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

extern unsigned int ConvertDXGIFormatToGraphicFormat(DXGI_FORMAT fmt);
extern DXGI_FORMAT ConvertGraphicFormatToDXGI(const Enigma::Graphics::GraphicFormat& format);

BackSurfaceDx11::BackSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D* tex, bool primary)
    : IBackSurface(name, primary)
{
    assert(device);
    m_dimension = MathLib::Dimension<unsigned>{ 0, 0 };
    m_d3dSurface = nullptr;
    if (!m_isPrimary)  // primary不要記錄在這裡
    {
        m_d3dSurface = tex;
    }
    m_d3dRenderTarget = nullptr;
    if (tex)
    {
        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);
        m_dimension.m_height = desc.Height;
        m_dimension.m_width = desc.Width;
        m_format.fmt = ConvertDXGIFormatToGraphicFormat(desc.Format);
    }
    if (tex) CreateD3DRenderTarget(device, tex);
}

BackSurfaceDx11::BackSurfaceDx11(const std::string& name, ID3D11Device* device, const MathLib::Dimension<unsigned>& dimension,
     const Enigma::Graphics::GraphicFormat& fmt) : IBackSurface(name, false)
{
    assert(device);
    m_dimension = MathLib::Dimension<unsigned>{ 0, 0 };
    m_d3dSurface = nullptr;
    m_d3dRenderTarget = nullptr;
    create(device, dimension, fmt);
    if (m_d3dSurface) CreateD3DRenderTarget(device, m_d3dSurface);
}

BackSurfaceDx11::~BackSurfaceDx11()
{
    SAFE_RELEASE(m_d3dRenderTarget);
    SAFE_RELEASE(m_d3dSurface);
}

error BackSurfaceDx11::Resize(const MathLib::Dimension<unsigned>& dimension)
{
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;
    SAFE_RELEASE(m_d3dSurface);
    SAFE_RELEASE(m_d3dRenderTarget);
    if (m_isPrimary)
    {
        if (FATAL_LOG_EXPR(!graphic->GetSwapChain())) return ErrorCode::swapChain;
        error er = graphic->GetSwapChain()->ResizeBackSurfaces(dimension);
        if (er) return er;
        m_dimension = dimension;
        m_d3dSurface = graphic->GetPrimaryD3DBackbuffer();
    }
    else
    {
        error er = create(graphic->GetD3DDevice(), dimension, m_format);
        if (er) return er;
    }
    CreateD3DRenderTarget(graphic->GetD3DDevice(), m_d3dSurface);
    Frameworks::EventPublisher::post(std::make_shared<Graphics::BackSurfaceResized>(m_name, m_dimension));

    return ErrorCode::ok;
}

void BackSurfaceDx11::CreateD3DRenderTarget(ID3D11Device* device, ID3D11Texture2D* texBackbuffer)
{
    assert(device);
    assert(texBackbuffer);

    HRESULT hr = device->CreateRenderTargetView(texBackbuffer, NULL, &m_d3dRenderTarget);
    if (FATAL_LOG_EXPR(FAILED(hr))) return;
}

error BackSurfaceDx11::create(ID3D11Device* device, const MathLib::Dimension<unsigned>& dimension,
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
    descDepth.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT hr = device->CreateTexture2D(&descDepth, NULL, &m_d3dSurface);
    if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::createBackSurfaceFail;

    m_dimension = dimension;
    m_format = fmt;

    return ErrorCode::ok;
}
