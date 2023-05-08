/*********************************************************************
 * \file   SwapChainDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef SWAP_CHAIN_DX11_H
#define SWAP_CHAIN_DX11_H

#include "MathLib/AlgebraBasicTypes.h"
#include <DXGI.h>
#include <system_error>

namespace Enigma::Devices
{
    using error = std::error_code;
    class DeviceCreatorDx11;

    class SwapChainDx11
    {
    public:
        SwapChainDx11(void);
        SwapChainDx11(const SwapChainDx11&) = delete;
        SwapChainDx11(SwapChainDx11&&) = delete;
        ~SwapChainDx11(void);
        SwapChainDx11& operator=(const SwapChainDx11&) = delete;
        SwapChainDx11& operator=(SwapChainDx11&&) = delete;

        void ToggleFullscreen(bool full_screen);

        IDXGISwapChain* GetDXGIObject() { return m_dxgiSwapChain; }

        error Present();

        error ResizeBackSurfaces(const MathLib::Dimension& dimension);

    protected:
        friend DeviceCreatorDx11;
        DXGI_SWAP_CHAIN_DESC m_currentDesc;
        IDXGISwapChain* m_dxgiSwapChain;
    };

};


#endif // SWAP_CHAIN_DX11_H
