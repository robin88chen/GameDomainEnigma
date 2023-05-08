/*********************************************************************
 * \file   DepthStencilSurfaceDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef DEPTH_STENCIL_SURFACED_X11_H
#define DEPTH_STENCIL_SURFACED_X11_H

#include <d3d11.h>
#include "GraphicKernel/IDepthStencilSurface.h"

namespace Enigma::Devices
{
    using error = std::error_code;
    class DepthStencilSurfaceDx11 : public Graphics::IDepthStencilSurface
    {
    public:
        DepthStencilSurfaceDx11(const std::string& name, ID3D11Device* device, const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        DepthStencilSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D* buff);
        DepthStencilSurfaceDx11(const std::string& name, const std::shared_ptr<DepthStencilSurfaceDx11>& depthDx11);
        DepthStencilSurfaceDx11(const DepthStencilSurfaceDx11&) = delete;
        DepthStencilSurfaceDx11(DepthStencilSurfaceDx11&&) = delete;
        virtual ~DepthStencilSurfaceDx11() override;
        DepthStencilSurfaceDx11& operator=(const DepthStencilSurfaceDx11&) = delete;
        DepthStencilSurfaceDx11& operator=(DepthStencilSurfaceDx11&&) = delete;

        virtual error Resize(const MathLib::Dimension& dimension) override;

        ID3D11DepthStencilView* GetD3DDepthView() { return m_d3dDepthView; }
    protected:
        error Create(ID3D11Device* device, const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        void CreateD3DDepthView(ID3D11Device* device);

    protected:
        ID3D11Texture2D* m_d3dSurface;
        ID3D11DepthStencilView* m_d3dDepthView;
    };
}


#endif // DEPTH_STENCIL_SURFACED_X11_H
