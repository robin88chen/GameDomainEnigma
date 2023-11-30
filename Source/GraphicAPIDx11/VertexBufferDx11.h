/*********************************************************************
 * \file   VertexBufferDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef VERTEX_BUFFER_DX11_H
#define VERTEX_BUFFER_DX11_H

#include "GraphicKernel/IVertexBuffer.h"
#include <D3D11.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class VertexBufferDx11 : public Graphics::IVertexBuffer
    {
    public:
        VertexBufferDx11(const std::string& name);
        VertexBufferDx11(const VertexBufferDx11&) = delete;
        VertexBufferDx11(VertexBufferDx11&&) = delete;
        virtual ~VertexBufferDx11() override;
        VertexBufferDx11& operator=(const VertexBufferDx11&) = delete;
        VertexBufferDx11& operator=(VertexBufferDx11&&) = delete;

        virtual error create(unsigned int sizeofVertex, unsigned int sizeBuffer) override;

        ID3D11Buffer* GetD3DBuffer() const { return m_d3dBuffer; }

    protected:
        virtual error UpdateBuffer(const byte_buffer& dataVertex) override;
        virtual error RangedUpdateBuffer(const ranged_buffer& buffer) override;

    protected:
        ID3D11Buffer* m_d3dBuffer;
    };
};

#endif // VERTEX_BUFFER_DX11_H
