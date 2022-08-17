#include "MultiBackSurfaceDx11.h"
#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <Platforms/MemoryAllocMacro.h>
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

extern unsigned int ConvertDXGIFormatToGraphicFormat(DXGI_FORMAT fmt);
extern DXGI_FORMAT ConvertGraphicFormatToDXGI(const Enigma::Graphics::GraphicFormat& format);

MultiBackSurfaceDx11::MultiBackSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D** tex, unsigned int buffer_count)
    : IMultiBackSurface(name)
{
    assert(device);
    assert(tex);
    assert(buffer_count > 1);

    m_formatArray.resize(buffer_count);
    m_surfaceCount = buffer_count;
    m_dimension = MathLib::Dimension{ 0, 0 };
    m_d3dSurfaceArray = memalloc(ID3D11Texture2D*, buffer_count);
    assert(m_d3dSurfaceArray);
    for (unsigned int bi = 0; bi < buffer_count; bi++)
    {
        m_d3dSurfaceArray[bi] = tex[bi];
        D3D11_TEXTURE2D_DESC desc;
        tex[bi]->GetDesc(&desc);
        if (bi == 0)
        {
            m_dimension.m_height = desc.Height;
            m_dimension.m_width = desc.Width;
            m_format.fmt = ConvertDXGIFormatToGraphicFormat(desc.Format);
        }
        m_formatArray[bi].fmt = ConvertDXGIFormatToGraphicFormat(desc.Format);
    }
    m_d3dRenderTargetArray = nullptr;
    CreateD3DRenderTarget(device, tex, buffer_count);
}

MultiBackSurfaceDx11::MultiBackSurfaceDx11(const std::string& name, ID3D11Device* device, const MathLib::Dimension& dimension,
    unsigned int buffer_count, const std::vector<Graphics::GraphicFormat>& fmt) : IMultiBackSurface(name)
{
    assert(device);
    assert(buffer_count > 1);
    m_formatArray.resize(buffer_count);
    m_surfaceCount = buffer_count;
    m_dimension = MathLib::Dimension{ 0, 0 };
    m_d3dSurfaceArray = nullptr;
    m_d3dRenderTargetArray = nullptr;
    Create(device, dimension, buffer_count, fmt);
    if (m_d3dSurfaceArray) CreateD3DRenderTarget(device, m_d3dSurfaceArray, buffer_count);
}

MultiBackSurfaceDx11::~MultiBackSurfaceDx11()
{
    ReleaseRenderTargetArray();
    SAFE_FREE(m_d3dRenderTargetArray);
    ReleaseD3DSurfaceArray();
    SAFE_FREE(m_d3dSurfaceArray);
}

error MultiBackSurfaceDx11::Resize(const MathLib::Dimension& dimension)
{
    if (FATAL_LOG_EXPR(m_isPrimary)) return ErrorCode::resizeBackSurfaceFail;
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    ReleaseD3DSurfaceArray();
    SAFE_FREE(m_d3dSurfaceArray);
    ReleaseRenderTargetArray();
    SAFE_FREE(m_d3dRenderTargetArray);

    error er = Create(graphic->GetD3DDevice(), dimension, m_surfaceCount, m_formatArray);
    if (er) return er;

    CreateD3DRenderTarget(graphic->GetD3DDevice(), m_d3dSurfaceArray, m_surfaceCount);
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::BackSurfaceResized{ m_name, m_dimension } });

    return ErrorCode::ok;
}

ID3D11Texture2D* MultiBackSurfaceDx11::GetD3DSurface(unsigned int index)
{
    if (FATAL_LOG_EXPR(index >= m_surfaceCount)) return nullptr;
    if (FATAL_LOG_EXPR(!m_d3dSurfaceArray)) return nullptr;
    return m_d3dSurfaceArray[index];
}

ID3D11Texture2D** MultiBackSurfaceDx11::GetD3DSurfaceArray()
{
    return m_d3dSurfaceArray;
}

void MultiBackSurfaceDx11::ReleaseD3DSurface(unsigned int index)
{
    if (FATAL_LOG_EXPR(index >= m_surfaceCount)) return;
    if (FATAL_LOG_EXPR(!m_d3dSurfaceArray)) return;
    SAFE_RELEASE(m_d3dSurfaceArray[index]);
}

void MultiBackSurfaceDx11::ReleaseD3DSurfaceArray()
{
    if ((m_surfaceCount) && (m_d3dSurfaceArray))
    {
        for (unsigned int i = 0; i < m_surfaceCount; i++)
        {
            SAFE_RELEASE(m_d3dSurfaceArray[i]);
        }
    }
}

ID3D11RenderTargetView* MultiBackSurfaceDx11::GetD3DRenderTarget(unsigned int index)
{
    if (FATAL_LOG_EXPR(index >= m_surfaceCount)) return nullptr;
    if (FATAL_LOG_EXPR(!m_d3dRenderTargetArray)) return nullptr;
    return m_d3dRenderTargetArray[index];
}

ID3D11RenderTargetView** MultiBackSurfaceDx11::GetD3DRenderTargetArray()
{
    return m_d3dRenderTargetArray;
}

void MultiBackSurfaceDx11::ReleaseRenderTarget(unsigned int index)
{
    if (FATAL_LOG_EXPR(index >= m_surfaceCount)) return;
    if (FATAL_LOG_EXPR(!m_d3dRenderTargetArray)) return;
    SAFE_RELEASE(m_d3dRenderTargetArray[index]);
}

void MultiBackSurfaceDx11::ReleaseRenderTargetArray()
{
    if ((m_surfaceCount) && (m_d3dRenderTargetArray))
    {
        for (unsigned int i = 0; i < m_surfaceCount; i++)
        {
            SAFE_RELEASE(m_d3dRenderTargetArray[i]);
        }
    }
}

void MultiBackSurfaceDx11::CreateD3DRenderTarget(ID3D11Device* device,
    ID3D11Texture2D** texBackbuffer, unsigned int buff_count)
{
    assert(device);
    assert(texBackbuffer);
    assert(buff_count > 1);

    ReleaseRenderTargetArray();
    SAFE_FREE(m_d3dRenderTargetArray);

    m_surfaceCount = buff_count;
    m_d3dRenderTargetArray = memalloc(ID3D11RenderTargetView*, buff_count);
    assert(m_d3dRenderTargetArray);
    ZeroMemory(m_d3dRenderTargetArray, buff_count * sizeof(ID3D11RenderTargetView*));

    for (unsigned int i = 0; i < buff_count; i++)
    {
        HRESULT hr = device->CreateRenderTargetView(texBackbuffer[i], nullptr, &m_d3dRenderTargetArray[i]);
        if (FATAL_LOG_EXPR(FAILED(hr)))  return;
    }
}

error MultiBackSurfaceDx11::Create(ID3D11Device* device, const MathLib::Dimension& dimension, 
    unsigned int buff_count, const std::vector<Graphics::GraphicFormat>& fmt)
{
    assert(device);
    assert(buff_count > 1);
    ReleaseD3DSurfaceArray();
    SAFE_FREE(m_d3dSurfaceArray);

    m_surfaceCount = buff_count;
    m_d3dSurfaceArray = memalloc(ID3D11Texture2D*, buff_count);
    assert(m_d3dSurfaceArray);
    ZeroMemory(m_d3dSurfaceArray, buff_count * sizeof(ID3D11Texture2D*));

    D3D11_TEXTURE2D_DESC descTexture;

    descTexture.Width = dimension.m_width;
    descTexture.Height = dimension.m_height;
    descTexture.MipLevels = 1;
    descTexture.ArraySize = 1;
    descTexture.Format = ConvertGraphicFormatToDXGI(fmt[0]);
    descTexture.SampleDesc.Count = 1;
    descTexture.SampleDesc.Quality = 0;
    descTexture.Usage = D3D11_USAGE_DEFAULT;
    descTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    descTexture.CPUAccessFlags = 0;
    descTexture.MiscFlags = 0;

    for (unsigned int i = 0; i < buff_count; i++)
    {
        descTexture.Format = ConvertGraphicFormatToDXGI(fmt[i]);
        HRESULT hr = device->CreateTexture2D(&descTexture, NULL, &m_d3dSurfaceArray[i]);
        if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::createBackSurfaceFail;
        m_formatArray[i] = fmt[i];
    }

    m_dimension = dimension;
    m_format = fmt[0];

    return ErrorCode::ok;
}
