/*********************************************************************
 * \file   GraphicAPIDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_DX11_H
#define GRAPHIC_API_DX11_H

#include "IGraphicAPI.h"

namespace Enigma::Graphics
{
    class DeviceCreatorDx11;
    class SwapChainDx11;
    class AdapterDx11;

    class GraphicAPIDx11 : public IGraphicAPI
    {
    public:
        GraphicAPIDx11();
        GraphicAPIDx11(const GraphicAPIDx11&) = delete;
        virtual ~GraphicAPIDx11() override;
        GraphicAPIDx11& operator=(const GraphicAPIDx11&) = delete;

        /** @name create / cleanup device */
        //@{
        virtual error CreateDevice(const DeviceRequiredBits& rqb, AsyncType use_async, void* hwnd) override;
        virtual error CleanupDevice() override;

    protected:
        DeviceCreatorDx11* m_creator;
        SwapChainDx11* m_swapChain;
        AdapterDx11* m_adapter;
    };
}

#endif // GRAPHIC_API_DX11_H
