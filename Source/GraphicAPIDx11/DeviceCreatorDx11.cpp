#include "DeviceCreatorDx11.h"
#include "AdapterDx11.h"
#include "SwapChainDx11.h"
#include "GraphicErrors.h"
#include "PlatformLayer.h"
#include <cassert>
#include <comdef.h>
#include <d3d11_1.h>


using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

const char* DXGIFormatToString(DXGI_FORMAT format);

DeviceCreatorDx11::DeviceCreatorDx11()
{
    m_wnd = 0;
    m_dxgiFactory = nullptr;
}

DeviceCreatorDx11::~DeviceCreatorDx11()
{
    SAFE_RELEASE(m_dxgiFactory);
}

error DeviceCreatorDx11::Initialize(HWND hwnd, const Graphics::DeviceRequiredBits& rqb)
{
    assert(!m_dxgiFactory);
    assert(hwnd);

    m_wnd = hwnd;
    m_deviceRqb = rqb;

    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&m_dxgiFactory));
    if (FAILED(hr))
    {
        return ErrorCode::DxgiInitialize;
    }
    assert(m_dxgiFactory);
    m_dxgiFactory->MakeWindowAssociation(hwnd, 0);
    return ErrorCode::ok;
}

error DeviceCreatorDx11::CreateWindowedDevice(AdapterDx11* adapter, SwapChainDx11* swapChain,
    ID3D11Device** d3dDevice, ID3D11DeviceContext** d3dDeviceContext)
{
    assert(adapter);
    assert(swapChain);
    assert(d3dDevice);
    if (FATAL_LOG_EXPR(!m_wnd)) return ErrorCode::InvalidWindow;
    if (FATAL_LOG_EXPR(!m_dxgiFactory)) return ErrorCode::DxgiInitialize;
    if (FATAL_LOG_EXPR(adapter->GetAdapterCount() <= 0)) return ErrorCode::AdapterFail;

    //adapter->SelectAdapter(0);

    RECT rc;
    GetClientRect(m_wnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0; //D3D10_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE;
#endif

    UINT const maxFeatureLevels = 7;
    D3D_FEATURE_LEVEL const featureLevels[maxFeatureLevels] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    UINT numFeatureLevels = 1;
    for (UINT i = 0; i < maxFeatureLevels; ++i, ++numFeatureLevels)
    {
        if (m_deviceRqb.m_featureLevel == featureLevels[i])
        {
            break;
        }
    }

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

    DXGI_SWAP_CHAIN_DESC* sd = &swapChain->m_currentDesc;
    ZeroMemory(sd, sizeof(sd));
    sd->BufferCount = 1; // 2;
    sd->BufferDesc.Width = width;
    sd->BufferDesc.Height = height;
    sd->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd->BufferDesc.RefreshRate.Numerator = 60; // 0;
    sd->BufferDesc.RefreshRate.Denominator = 1;
    sd->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd->OutputWindow = m_wnd;
    sd->SampleDesc.Count = 1;
    sd->SampleDesc.Quality = 0;
    sd->Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevelGot;
    HRESULT hr;
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        m_d3dDriverType = driverTypes[driverTypeIndex];
        // Create the device and device context.
        /*if (adapter->GetSelectedAdapterInfo()->m_dxgiAdapter && m_d3dDriverType != D3D_DRIVER_TYPE_UNKNOWN)
        {
            m_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
            Logger::Warning("Non-null adapter requires D3D_DRIVER_TYPE_UNKNOWN.");
        }*/
        hr = D3D11CreateDeviceAndSwapChain(nullptr, /*adapter->GetSelectedAdapterInfo()->m_dxgiAdapter,*/
            m_d3dDriverType, NULL, createDeviceFlags,
            featureLevels, numFeatureLevels, D3D11_SDK_VERSION, sd, &swapChain->m_dxgiSwapChain, d3dDevice, &featureLevelGot, d3dDeviceContext);
        /*if (swapChain->m_dxgiSwapChain)
        {
            swapChain->m_dxgiSwapChain->GetDesc(&swapChain->m_currentDesc);
        }*/

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
    {
        Platforms::Debug::Printf("Failed HResult = %x\n", hr);
        LOG(Error, make_error_code(ErrorCode::CreateDeviceFail).message());
        return ErrorCode::CreateDeviceFail;
    }

    // Store device description
    std::string deviceStats = "Device Created: ";
    deviceStats += "Adapter " + std::string(adapter->GetSelectedAdapterInfo()->m_uniqueDescription);
    deviceStats += " Driver Type ";
    if (m_d3dDriverType == D3D_DRIVER_TYPE_UNKNOWN) deviceStats += "UNKNOWN";
    else if (m_d3dDriverType == D3D_DRIVER_TYPE_HARDWARE) deviceStats += "HARDWARE";
    else if (m_d3dDriverType == D3D_DRIVER_TYPE_REFERENCE) deviceStats += "REFERENCE";
    char s[64];
    if (swapChain->m_dxgiSwapChain)
    {
        DXGI_SWAP_CHAIN_DESC desc;
        swapChain->m_dxgiSwapChain->GetDesc(&desc);
        sprintf_s(s, " Buffer Count %d ", desc.BufferCount);
        deviceStats += s;
        sprintf_s(s, "(%d x %d) ", desc.BufferDesc.Width, desc.BufferDesc.Height);
        deviceStats += s;
        deviceStats += DXGIFormatToString(desc.BufferDesc.Format);
        sprintf_s(s, " Freq : %d / %d ", desc.BufferDesc.RefreshRate.Numerator, desc.BufferDesc.RefreshRate.Denominator);
        deviceStats += s;
    }
    sprintf_s(s, " Feature Level %X", featureLevelGot);
    deviceStats += s;
    LOG(Info, deviceStats);

    return ErrorCode::ok;
}

error DeviceCreatorDx11::BuildDeviceList(AdapterDx11* adapter)
{
    assert(adapter);
    if (FATAL_LOG_EXPR(!m_dxgiFactory)) return ErrorCode::DxgiInitialize;

    IDXGIAdapter1* dxgiAdapter;
    unsigned int i = 0;

    while (m_dxgiFactory->EnumAdapters1(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        AdapterDx11::AdapterInfo* adapterInfo = menew AdapterDx11::AdapterInfo();
        adapterInfo->m_dxgiAdapter = dxgiAdapter;
        adapterInfo->m_adapterOrdinal = i;
        dxgiAdapter->GetDesc1(&adapterInfo->m_desc);
        unsigned int wlen = lstrlenW(adapterInfo->m_desc.Description);
        int mblen = WideCharToMultiByte(CP_ACP, 0, adapterInfo->m_desc.Description, wlen, NULL, 0, NULL, NULL);
        char* description = menew char[mblen + 8];
        memset(description, 0, mblen + 8);
        WideCharToMultiByte(CP_ACP, 0, adapterInfo->m_desc.Description, wlen, description, mblen + 8, NULL, NULL);
        adapterInfo->m_uniqueDescription = menew char[mblen + 32];
        sprintf_s(adapterInfo->m_uniqueDescription, mblen + 32, "%s (#%d)", description, i);
        medelete[] description;

        error er = EnumerateDevices(adapterInfo);
        if (er) return er;

        er = EnumerateOutputs(adapterInfo);
        if (er) return er;

        adapter->m_adapterInfoList.push_back(adapterInfo);
        ++i;
    }

    return ErrorCode::ok;
}

error DeviceCreatorDx11::EnumerateDevices(AdapterDx11::AdapterInfo* adapterInfo)
{
    const D3D_DRIVER_TYPE devTypeArray[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        //D3D10_DRIVER_TYPE_REFERENCE,
    };
    const UINT devTypeArrayCount = sizeof(devTypeArray) / sizeof(devTypeArray[0]);

    // Enumerate each Direct3D device type
    for (UINT iDeviceType = 0; iDeviceType < devTypeArrayCount; iDeviceType++)
    {
        AdapterDx11::DeviceInfo* device_info = menew AdapterDx11::DeviceInfo;

        device_info->m_d3dDriverType = devTypeArray[iDeviceType];
        device_info->m_adapterOrdinal = adapterInfo->m_adapterOrdinal;

        // Call D3D10CreateDevice to ensure that this is a D3D11 device.
        ID3D11Device* d3dDevice = NULL;
        IDXGIAdapter1* adapter = NULL;
        if (devTypeArray[iDeviceType] == D3D10_DRIVER_TYPE_HARDWARE)
            adapter = adapterInfo->m_dxgiAdapter;
        UINT createDeviceFlags = 0; //D3D10_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
        createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
        HRESULT hr = D3D11CreateDevice(adapter, devTypeArray[iDeviceType], 0, createDeviceFlags,
            NULL, 0, D3D11_SDK_VERSION, &d3dDevice, NULL, NULL);
        if (FAILED(hr))
        {
            medelete device_info;
            continue;
        }

        if (devTypeArray[iDeviceType] != D3D10_DRIVER_TYPE_HARDWARE)
        {
            IDXGIDevice* pDXGIDev = NULL;
            hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (LPVOID*)&pDXGIDev);
            if (SUCCEEDED(hr) && pDXGIDev)
            {
                IDXGIAdapter* dxgiAdapter0;
                SAFE_RELEASE(adapterInfo->m_dxgiAdapter);
                pDXGIDev->GetAdapter(&dxgiAdapter0);
                if (dxgiAdapter0)
                {
                    dxgiAdapter0->QueryInterface<IDXGIAdapter1>(&adapterInfo->m_dxgiAdapter);
                    SAFE_RELEASE(dxgiAdapter0);
                }
                //pDXGIDev->GetAdapter(&adapterInfo->m_dxgiAdapter);
            }
            SAFE_RELEASE(pDXGIDev);
        }

        SAFE_RELEASE(d3dDevice);
        adapterInfo->m_deviceInfoList.push_back(device_info);
    }

    return ErrorCode::ok;
}

error DeviceCreatorDx11::EnumerateOutputs(AdapterDx11::AdapterInfo* adapterInfo)
{
    HRESULT hr;
    IDXGIOutput* dxgiOutput;

    for (int iOutput = 0;; ++iOutput)
    {
        dxgiOutput = NULL;
        hr = adapterInfo->m_dxgiAdapter->EnumOutputs(iOutput, &dxgiOutput);
        if (DXGI_ERROR_NOT_FOUND == hr)
        {
            return ErrorCode::ok;
        }
        else if (FAILED(hr))
        {
            return ErrorCode::EnumerateDevicesFail; //Something bad happened.
        }
        else //Success!
        {
            AdapterDx11::OutputInfo* outputInfo = menew AdapterDx11::OutputInfo();

            outputInfo->m_adapterOrdinal = adapterInfo->m_adapterOrdinal;
            outputInfo->m_dxgiOutput = dxgiOutput;
            outputInfo->m_outputOrdinal = iOutput;
            dxgiOutput->GetDesc(&outputInfo->m_desc);

            unsigned int wlen = lstrlenW(outputInfo->m_desc.DeviceName);
            int mblen = WideCharToMultiByte(CP_ACP, 0, outputInfo->m_desc.DeviceName, wlen, NULL, 0, NULL, NULL);
            char* description = menew char[mblen + 8];
            memset(description, 0, mblen + 8);
            WideCharToMultiByte(CP_ACP, 0, outputInfo->m_desc.DeviceName, wlen, description, mblen + 8, NULL, NULL);
            outputInfo->m_uniqueDescription = menew char[mblen + 32];
            sprintf_s(outputInfo->m_uniqueDescription, mblen + 32, "%s (#%d)", description, iOutput);
            medelete[] description;

            EnumerateDisplayModes(outputInfo);
            if (outputInfo->m_displayModeDescList.size() <= 0)
            {
                // If this output has no valid display mode, do not save it.
                delete outputInfo;
                continue;
            }

            adapterInfo->m_outputInfoList.push_back(outputInfo);
        }
    }

    // un-reached code
}

error DeviceCreatorDx11::EnumerateDisplayModes(AdapterDx11::OutputInfo* outputInfo)
{
    HRESULT hr = S_OK;
    const DXGI_FORMAT allowedAdapterFormatArray[] =
    {
        DXGI_FORMAT_R8G8B8A8_UNORM,         //This is preferred mode

        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    };
    int allowedAdapterFormatArrayCount = sizeof(allowedAdapterFormatArray) / sizeof(allowedAdapterFormatArray[0]);

    for (int f = 0; f < allowedAdapterFormatArrayCount; ++f)
    {
        // Fast-path: Try to grab at least 512 modes.
        //            This is to avoid calling GetDisplayModeList more times than necessary.
        //            GetDisplayModeList is an expensive call.
        UINT numModes = 512;
        DXGI_MODE_DESC* descs = menew DXGI_MODE_DESC[numModes];
        hr = outputInfo->m_dxgiOutput->GetDisplayModeList(allowedAdapterFormatArray[f],
            0,
            &numModes,
            descs);
        if (DXGI_ERROR_NOT_FOUND == hr)
        {
            SAFE_DELETE_ARRAY(descs);
            numModes = 0;
            break;
        }
        else if (MAKE_DXGI_HRESULT(34) == hr && DXGI_FORMAT_R8G8B8A8_UNORM == allowedAdapterFormatArray[f])
        {
            // DXGI cannot enumerate display modes over a remote session.  Therefore, create a fake display
            // mode for the current screen resolution for the remote session.
            if (0 != GetSystemMetrics(0x1000)) // SM_REMOTESESSION
            {
                DEVMODE DevMode;
                DevMode.dmSize = sizeof(DEVMODE);
                if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode))
                {
                    numModes = 1;
                    descs[0].Width = DevMode.dmPelsWidth;
                    descs[0].Height = DevMode.dmPelsHeight;
                    descs[0].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    descs[0].RefreshRate.Numerator = 60;
                    descs[0].RefreshRate.Denominator = 1;
                    descs[0].ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
                    descs[0].Scaling = DXGI_MODE_SCALING_CENTERED;
                    hr = S_OK;
                }
            }
        }
        else if (DXGI_ERROR_MORE_DATA == hr)
        {
            // Slow path.  There were more than 512 modes.
            SAFE_DELETE_ARRAY(descs);
            hr = outputInfo->m_dxgiOutput->GetDisplayModeList(allowedAdapterFormatArray[f],
                0,
                &numModes,
                NULL);
            if (FAILED(hr))
            {
                SAFE_DELETE_ARRAY(descs);
                numModes = 0;
                break;
            }

            descs = menew DXGI_MODE_DESC[numModes];

            hr = outputInfo->m_dxgiOutput->GetDisplayModeList(allowedAdapterFormatArray[f],
                0,
                &numModes,
                descs);
            if (FAILED(hr))
            {
                SAFE_DELETE_ARRAY(descs);
                numModes = 0;
                break;
            }
        }

        if (0 == numModes && 0 == f)
        {
            // No R8G8B8A8_UNORM modes!
            // Abort the fast-path if we're on it
            allowedAdapterFormatArrayCount = sizeof(allowedAdapterFormatArray) / sizeof(allowedAdapterFormatArray[0]);
            SAFE_DELETE_ARRAY(descs);
            continue;
        }

        if (SUCCEEDED(hr))
        {
            unsigned int cur_size = (unsigned int)outputInfo->m_displayModeDescList.size();
            outputInfo->m_displayModeDescList.resize(cur_size + numModes);
            memcpy_s(&(outputInfo->m_displayModeDescList[cur_size]), numModes * sizeof(DXGI_MODE_DESC), descs, numModes * sizeof(DXGI_MODE_DESC));
        }

        SAFE_DELETE_ARRAY(descs);
    }
    return ErrorCode::ok;
}
