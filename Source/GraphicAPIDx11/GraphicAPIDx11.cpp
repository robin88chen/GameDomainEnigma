#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "AdapterDx11.h"
#include "DeviceCreatorDx11.h"
#include "GraphicErrors.h"
#include "MemoryAllocMacro.h"
#include "MemoryMacro.h"
#include "PlatformLayer.h"

using namespace Enigma::Devices;

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

error GraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, AsyncType use_async, void* hwnd)
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
