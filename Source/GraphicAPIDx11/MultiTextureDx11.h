/*********************************************************************
 * \file   MultiTextureDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef MULTI_TEXTURE_DX11_H
#define MULTI_TEXTURE_DX11_H

#include "GraphicKernel/IMultiTexture.h"
#include <D3D11.h>
#include "DirectXTex.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class MultiTextureDx11 : public Graphics::IMultiTexture
    {
    public:
        MultiTextureDx11(const std::string& name);
        MultiTextureDx11(const MultiTextureDx11&) = delete;
        MultiTextureDx11(MultiTextureDx11&&) = delete;
        virtual ~MultiTextureDx11();
        MultiTextureDx11& operator=(const MultiTextureDx11&) = delete;
        MultiTextureDx11& operator=(MultiTextureDx11&&) = delete;

        virtual error LoadTextureImages(const std::vector<byte_buffer>& img_buffs) override;
        virtual error SaveTextureImages(const std::vector<FileSystem::IFilePtr>& files) override;

        virtual error UseAsBackSurface(const Graphics::IBackSurfacePtr& back_surf) override;

        //virtual void LoadImageDimension(const std::string& filename, const std::string& path_id) override;

        ID3D11ShaderResourceView* GetD3DResourceView(unsigned int idx) const;
        ID3D11ShaderResourceView** GetD3DResourceViewArray() const;
        unsigned int GetResourceViewCount() const;

    protected:
        error CreateFromScratchImage(unsigned int index, DirectX::ScratchImage& scratchImage);
        virtual error CreateOneFromSystemMemory(unsigned int index, const MathLib::Dimension& dimension,
            const byte_buffer& buff) override;

    protected:
        ID3D11ShaderResourceView** m_d3dTextureResources;
        unsigned int m_resourceViewCount;
    };
}


#endif // MULTI_TEXTURE_DX11_H
