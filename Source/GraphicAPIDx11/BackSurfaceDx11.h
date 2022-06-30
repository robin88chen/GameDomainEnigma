/*********************************************************************
 * \file   BackSurfaceDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef BACK_SURFACE_DX11_H
#define BACK_SURFACE_DX11_H

#include "GraphicKernel/IBackSurface.h"
#include <D3D11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class BackSurfaceDx11 : public Graphics::IBackSurface
    {
    public:
        BackSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D* tex, bool primary);
        BackSurfaceDx11(const std::string& name, ID3D11Device* device, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        BackSurfaceDx11(const BackSurfaceDx11&) = delete;
        virtual ~BackSurfaceDx11();
        BackSurfaceDx11& operator=(const BackSurfaceDx11&) = delete;

        virtual error Resize(const MathLib::Dimension& dimension) override;

        ID3D11RenderTargetView* GetD3DRenderTarget() { return m_d3dRenderTarget; }
        ID3D11Texture2D* GetD3DSurface() { return m_d3dSurface; }

    protected:
        void CreateD3DRenderTarget(ID3D11Device* device, ID3D11Texture2D* texBackbuffer);
        error Create(ID3D11Device* device, const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);

    protected:
        ID3D11Texture2D* m_d3dSurface;
        ID3D11RenderTargetView* m_d3dRenderTarget;
    };

}

#endif // BACK_SURFACE_DX11_H
