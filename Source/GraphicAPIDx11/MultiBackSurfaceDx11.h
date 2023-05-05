/*********************************************************************
 * \file   MultiBackSurfaceDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef MULTI_BACKSURFACE_DX11_H
#define MULTI_BACKSURFACE_DX11_H

#include "GraphicKernel/IMultiBackSurface.h"
#include <D3D11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class MultiBackSurfaceDx11 : public Graphics::IMultiBackSurface
    {
    public:
        MultiBackSurfaceDx11(const std::string& name, ID3D11Device* device, ID3D11Texture2D** tex, unsigned int buffer_count);  // not support for primary BB
        MultiBackSurfaceDx11(const std::string& name, ID3D11Device* device, const MathLib::Dimension& dimension,
            unsigned int buffer_count, const std::vector<Graphics::GraphicFormat>& fmt);
        MultiBackSurfaceDx11(const MultiBackSurfaceDx11&) = delete;
        MultiBackSurfaceDx11(MultiBackSurfaceDx11&&) = delete;
        virtual ~MultiBackSurfaceDx11() override;
        MultiBackSurfaceDx11& operator=(const MultiBackSurfaceDx11&) = delete;
        MultiBackSurfaceDx11& operator=(MultiBackSurfaceDx11&&) = delete;

        virtual error Resize(const MathLib::Dimension& dimension) override;

        ID3D11Texture2D* GetD3DSurface(unsigned int index);
        ID3D11Texture2D** GetD3DSurfaceArray();
        void ReleaseD3DSurface(unsigned int index);
        void ReleaseD3DSurfaceArray();
        ID3D11RenderTargetView* GetD3DRenderTarget(unsigned int index);
        ID3D11RenderTargetView** GetD3DRenderTargetArray();
        void ReleaseRenderTarget(unsigned int index);
        void ReleaseRenderTargetArray();

    protected:
        void CreateD3DRenderTarget(ID3D11Device* device, ID3D11Texture2D** texBackbuffer, unsigned int buff_count);
        error Create(ID3D11Device* device, const MathLib::Dimension& dimension,
            unsigned int buff_count, const std::vector<Graphics::GraphicFormat>& fmt);

    protected:
        ID3D11Texture2D** m_d3dSurfaceArray;
        ID3D11RenderTargetView** m_d3dRenderTargetArray;
    };
};


#endif // MULTI_BACKSURFACE_DX11_H
