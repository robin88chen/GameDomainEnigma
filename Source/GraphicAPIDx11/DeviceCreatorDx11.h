/*********************************************************************
 * \file   DeviceCreatorDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef DEVICE_CREATOR_DX11_H
#define DEVICE_CREATOR_DX11_H

#include <dxgi.h>
#include <d3d11.h>
#include <system_error>
#include <vector>
#include "AdapterDx11.h"
#include "DeviceRequiredBits.h"

namespace Enigma::Devices
{
    using error = std::error_code;
    class AdapterDx11;
    class SwapChainDx11;

    class DeviceCreatorDx11
    {
    public:
        DeviceCreatorDx11();
        DeviceCreatorDx11(const DeviceCreatorDx11&) = delete;
        ~DeviceCreatorDx11();
        DeviceCreatorDx11& operator=(const DeviceCreatorDx11&) = delete;

        error Initialize(HWND hwnd, const Graphics::DeviceRequiredBits& rqb);
        IDXGIFactory* GetDxgiFactory() { return m_dxgiFactory; };

        error BuildDeviceList(AdapterDx11* adapter);
        error CreateWindowedDevice(AdapterDx11* adapter, SwapChainDx11* swapChain,
            ID3D11Device** d3dDevice, ID3D11DeviceContext** d3dDeviceContext);

    protected:
        error EnumerateDevices(AdapterDx11::AdapterInfo* pAdapterInfo);
        error EnumerateOutputs(AdapterDx11::AdapterInfo* pAdapterInfo);
        error EnumerateDisplayModes(AdapterDx11::OutputInfo* pOutputInfo);

    protected:
        HWND m_wnd;
        IDXGIFactory1* m_dxgiFactory;
        D3D_DRIVER_TYPE m_d3dDriverType;
        Graphics::DeviceRequiredBits m_deviceRqb;

        std::vector<DXGI_SWAP_CHAIN_DESC> m_SwapChainDescList;
    };
};


#endif // DEVICE_CREATOR_DX11_H
