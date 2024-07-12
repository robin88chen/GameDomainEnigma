﻿/*********************************************************************
 * \file   RenderBuffer.h
 * \brief  Render buffer entity
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "RenderBufferSignature.h"
#include "Geometries/GeometrySegment.h"
#include "GraphicKernel/IVertexBuffer.h"
#include "GraphicKernel/IIndexBuffer.h"
#include <system_error>
#include <optional>

namespace Enigma::Engine
{
    using error = std::error_code;
    class EffectMaterial;
    /** render buffer */
    class RenderBuffer
    {
    public:
        RenderBuffer(const RenderBufferSignature& signature,
            const Graphics::IVertexBufferPtr& vertex_buffer, const Graphics::IIndexBufferPtr& index_buffer);
        RenderBuffer(const RenderBuffer&) = delete;
        RenderBuffer(RenderBuffer&&) = delete;
        virtual ~RenderBuffer();
        RenderBuffer& operator=(const RenderBuffer&) = delete;
        RenderBuffer& operator=(RenderBuffer&&) = delete;

        /** update vertex */
        error updateVertex(const byte_buffer& dataBuffer, const uint_buffer& indexBuffer);
        /** ranged update vertex */
        error rangedUpdateVertex(const Graphics::IVertexBuffer::ranged_buffer& vtxBuffer,
            const std::optional<const Graphics::IIndexBuffer::ranged_buffer>& idxBuffer);

        Graphics::IVertexBufferPtr vertexBuffer() { return m_vertexBuffer; };
        Graphics::IIndexBufferPtr indexBuffer() { return m_indexBuffer; };

        /** get signature */
        const RenderBufferSignature& signature() const { return m_signature; };
        RenderBufferSignature signature() { return m_signature; }

        bool isDataEmpty() { return m_isDataEmpty; };

        /** draw */
        error draw(const std::shared_ptr<EffectMaterial>& effectMaterial, const Geometries::GeometrySegment& segment);

    protected:
        // todo: 這些func的 Async 由 Manager 負責以 Async 方式呼叫

    protected:
        RenderBufferSignature m_signature;
        Graphics::IVertexBufferPtr m_vertexBuffer;
        Graphics::IIndexBufferPtr m_indexBuffer;

        bool m_isDataEmpty;
    };
    using RenderBufferPtr = std::shared_ptr<RenderBuffer>;
}

#endif // RENDER_BUFFER_H
