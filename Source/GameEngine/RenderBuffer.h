/*********************************************************************
 * \file   RenderBuffer.h
 * \brief  Render buffer entity
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "RenderBufferSignature.h"
#include "EffectMaterial.h"
#include "GeometrySegment.h"
#include "GraphicKernel/IVertexBuffer.h"
#include "GraphicKernel/IIndexBuffer.h"
#include "MathLib/Matrix4.h"
#include <system_error>
#include <optional>

namespace Enigma::Engine
{
    using error = std::error_code;
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
        error UpdateVertex(const byte_buffer& dataBuffer, const uint_buffer& indexBuffer);
        /** ranged update vertex */
        error RangedUpdateVertex(const Graphics::IVertexBuffer::ranged_buffer& vtxBuffer,
            const std::optional<const Graphics::IIndexBuffer::ranged_buffer>& idxBuffer);

        Graphics::IVertexBufferPtr GetVertexBuffer() { return m_vertexBuffer; };
        Graphics::IIndexBufferPtr GetIndexBuffer() { return m_indexBuffer; };

        /** get vertex declaration */
        Graphics::IVertexDeclarationPtr GetVertexDeclaration() const
        {
            return m_signature.GetVertexDeclaration();
        };

        /** get signature */
        const RenderBufferSignature& GetSignature() const { return m_signature; };
        RenderBufferSignature GetSignature() { return m_signature; }

        bool IsDataEmpty() { return m_isDataEmpty; };

        /** draw */
        error Draw(const MathLib::Matrix4& mxWorld, const EffectMaterialPtr& effectMaterial,
            const GeometrySegment& segment);

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
