#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "AdapterDx11.h"
#include "DeviceCreatorDx11.h"
#include "BackSurfaceDx11.h"
#include "DepthStencilSurfaceDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

GraphicAPIDx11::GraphicAPIDx11() : IGraphicAPI()
{
    m_apiVersion = IGraphicAPI::APIVersion::API_Dx11;
    m_wnd = nullptr;
    m_creator = nullptr;
    m_swapChain = nullptr;
    m_adapter = nullptr;

    m_d3dDevice = nullptr;
    m_d3dDeviceContext = nullptr;
}

GraphicAPIDx11::~GraphicAPIDx11()
{
    SAFE_DELETE(m_creator);
    SAFE_DELETE(m_swapChain);
    SAFE_DELETE(m_adapter);
}

error GraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    Platforms::Debug::Printf("create dx11 device in thread %d\n", std::this_thread::get_id());
    m_deviceRequiredBits = rqb;
    m_wnd = hwnd;
    m_adapter = menew AdapterDx11();
    m_swapChain = menew SwapChainDx11();
    m_creator = menew DeviceCreatorDx11();
    m_creator->Initialize((HWND)m_wnd, rqb);
    m_creator->BuildDeviceList(m_adapter);

    SAFE_RELEASE(m_d3dDevice);

    error er = m_creator->CreateWindowedDevice(m_adapter, m_swapChain, &m_d3dDevice, &m_d3dDeviceContext);
    if (er) return er;
    AddDebugInfoFilter();
    return er;
}

error GraphicAPIDx11::CleanupDevice()
{
    Platforms::Debug::Printf("cleanup device in thread %d\n", std::this_thread::get_id());
    CleanupDeviceObjects();

    SAFE_RELEASE(m_d3dDevice);
    SAFE_RELEASE(m_d3dDeviceContext);
    return Graphics::ErrorCode::ok;
}

error GraphicAPIDx11::GetPrimaryBackSurface(Graphics::IBackSurfacePtr* back_surface, Graphics::IDepthStencilSurfacePtr* depth_surface)
{
    Platforms::Debug::Printf("get primary back surface in thread %d\n", std::this_thread::get_id());
    assert(back_surface);
    if (FATAL_LOG_EXPR(!m_d3dDevice)) return ErrorCode::d3dDeviceNullPointer;

    ID3D11Texture2D* d3dbackTex = GetPrimaryD3DBackbuffer();
    if (!d3dbackTex)
    {
        return ErrorCode::createBackSurfaceFail;
    }
    Graphics::IBackSurface* back = menew BackSurfaceDx11{ m_d3dDevice, d3dbackTex, true };
    back_surface->reset(back); // std::make_shared<IBackSurface>(back);
    //*back_surface = menew BackSurfaceDx11(m_graphicApi->GetD3DDevice(), d3dbackTex, true);
    SetPrimaryBackSurfaceFormat((*back_surface)->GetFormat());
    //if (depth_surface) *depth_surface = 0;

    if ((m_deviceRequiredBits.m_usesDepthBuffer) && (depth_surface))
    {
        auto dimension = (*back_surface)->GetDimension();
        depth_surface->reset(
            menew DepthStencilSurfaceDx11{ m_d3dDevice, dimension, Graphics::GraphicFormat { Graphics::GraphicFormat::FMT_D24S8} });
        //*depth_surface = CreateDepthStencilSurface(w, h, GraphicFormat(GraphicFormat::FMT_D24S8));
        SetDepthSurfaceFormat((*depth_surface)->GetFormat());
    }
    if (d3dbackTex) d3dbackTex->Release();

    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt, Graphics::IBackSurfacePtr* back_surface)
{
    Platforms::Debug::Printf("create back surface in thread %d\n", std::this_thread::get_id());
    assert(back_surface);
    back_surface->reset(menew BackSurfaceDx11{ GetD3DDevice(), dimension, fmt });
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt, Graphics::IDepthStencilSurfacePtr* depth_surface)
{
    Platforms::Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    assert(depth_surface);
    depth_surface->reset(menew DepthStencilSurfaceDx11{ GetD3DDevice(), dimension, fmt });
    return ErrorCode::ok;
}

error GraphicAPIDx11::ShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& from_depth, Graphics::IDepthStencilSurfacePtr* depth_surface)
{
    Platforms::Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    assert(depth_surface);
    DepthStencilSurfaceDx11* depth_dx11 = dynamic_cast<DepthStencilSurfaceDx11*>(from_depth.get());
    assert(depth_dx11);
    depth_surface->reset(menew DepthStencilSurfaceDx11{ depth_dx11 });
    return ErrorCode::ok;
}

void GraphicAPIDx11::CleanupDeviceObjects()
{
}

void GraphicAPIDx11::AddDebugInfoFilter()
{
    ID3D11Debug* d3dDebug = nullptr;
    if (SUCCEEDED(m_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {

            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
                D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
                // Add more message IDs here as needed
            };

            D3D11_INFO_QUEUE_FILTER filter;
            memset(&filter, 0, sizeof(filter));
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
}

ID3D11Texture2D* GraphicAPIDx11::GetPrimaryD3DBackbuffer()
{
    if (FATAL_LOG_EXPR((!m_swapChain) || (!m_swapChain->GetDXGIObject()))) return nullptr;
    ID3D11Texture2D* back_surface;
    HRESULT hr = m_swapChain->GetDXGIObject()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_surface);
    if (FAILED(hr))
    {
        return nullptr;
    }
    return back_surface;
}
