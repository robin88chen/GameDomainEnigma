/*********************************************************************
 * \file   IndexBufferDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef INDEX_BUFFER_DX11_H
#define INDEX_BUFFER_DX11_H

#include "GraphicKernel/IIndexBuffer.h"
#include <D3D11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class IndexBufferDx11 : public Graphics::IIndexBuffer
    {
    public:
        IndexBufferDx11(const std::string& name);
        IndexBufferDx11(const IndexBufferDx11&) = delete;
        IndexBufferDx11(IndexBufferDx11&&) = delete;
        virtual ~IndexBufferDx11() override;
        IndexBufferDx11& operator=(const IndexBufferDx11&) = delete;
        IndexBufferDx11& operator=(IndexBufferDx11&&) = delete;

        virtual error create(unsigned int sizeBuffer) override;

        ID3D11Buffer* GetD3DBuffer() const { return m_d3dBuffer; }

    protected:
        virtual error UpdateBuffer(const uint_buffer& dataIndex) override;
        virtual error RangedUpdateBuffer(const ranged_buffer& buffer) override;

    protected:
        ID3D11Buffer* m_d3dBuffer;
    };
};

#endif // INDEX_BUFFER_DX11_H
