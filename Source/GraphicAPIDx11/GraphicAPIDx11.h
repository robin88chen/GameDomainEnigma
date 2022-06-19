/*********************************************************************
 * \file   GraphicAPIDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_DX11_H
#define GRAPHIC_API_DX11_H

#include <D3D11.h>
#include "GraphicKernel/IGraphicAPI.h"

namespace Enigma::Graphics
{
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    class IDepthStencilSurface;
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;
}
namespace Enigma::Devices
{
    class DeviceCreatorDx11;
    class SwapChainDx11;
    class AdapterDx11;

    using error = std::error_code;

    class GraphicAPIDx11 : public Graphics::IGraphicAPI
    {
    public:
        GraphicAPIDx11();
        GraphicAPIDx11(const GraphicAPIDx11&) = delete;
        virtual ~GraphicAPIDx11() override;
        GraphicAPIDx11& operator=(const GraphicAPIDx11&) = delete;

        virtual error CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd) override;
        virtual error CleanupDevice() override;

        virtual error GetPrimaryBackSurface(Graphics::IBackSurfacePtr* back_surface, Graphics::IDepthStencilSurfacePtr* depth_surface) override;
        virtual error CreateBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt,
            Graphics::IBackSurfacePtr* back_surface) override;
        virtual error CreateBackSurface(const MathLib::Dimension& dimension, unsigned int buff_count,
            const std::vector<Graphics::GraphicFormat>& fmts, Graphics::IBackSurfacePtr* back_surface) override;
        virtual error CreateDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt,
                                                Graphics::IDepthStencilSurfacePtr* depth_surface) override;
        virtual error ShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& from_depth,
                                               Graphics::IDepthStencilSurfacePtr* depth_surface) override;
        virtual error ClearSurface(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) override;

        ID3D11Texture2D* GetPrimaryD3DBackbuffer();

        ID3D11Device* GetD3DDevice() { return m_d3dDevice; };
        ID3D11DeviceContext* GetD3DDeviceContext() { return m_d3dDeviceContext; }
        SwapChainDx11* GetSwapChain() { return m_swapChain; }

    private:
        void CleanupDeviceObjects();

        error ClearSingleBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color);
        error ClearMultiBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color);
        error ClearDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& depth_surface,
            float depth_value, unsigned int stencil_value);

        void AddDebugInfoFilter();

    private:
        DeviceCreatorDx11* m_creator;
        SwapChainDx11* m_swapChain;
        AdapterDx11* m_adapter;
        // D3D Device
        ID3D11Device* m_d3dDevice;
        ID3D11DeviceContext* m_d3dDeviceContext;
    };
}

#endif // GRAPHIC_API_DX11_H
