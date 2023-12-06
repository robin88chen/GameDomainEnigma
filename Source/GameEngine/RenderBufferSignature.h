/*********************************************************************
 * \file   RenderBufferSignature.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_SIGNATURE_H
#define RENDER_BUFFER_SIGNATURE_H

#include "GraphicKernel/GraphicAPITypes.h"
#include <string>

namespace Enigma::Engine
{
    class RenderBufferSignature
    {
    public:
        RenderBufferSignature();
        RenderBufferSignature(const std::string& name, Graphics::PrimitiveTopology pt,
            unsigned int vertexCapa, unsigned int idxCapa);
        RenderBufferSignature(const RenderBufferSignature& signature);
        RenderBufferSignature(RenderBufferSignature&& signature) noexcept;
        ~RenderBufferSignature();
        RenderBufferSignature& operator=(const RenderBufferSignature& signature);
        RenderBufferSignature& operator=(RenderBufferSignature&& signature) noexcept;

        bool operator==(const RenderBufferSignature& signature) const;
        bool operator!=(const RenderBufferSignature& signature) const;
        bool operator<(const RenderBufferSignature& signature) const;

        const std::string& getName() const { return m_name; };
        // 拿掉 vertex layout, 應該不需要
        //Graphics::IVertexDeclarationPtr GetVertexDeclaration() const { return m_vertexDecl.lock(); };
        const Graphics::PrimitiveTopology GetTopology() const { return m_topology; };
        unsigned int getVertexCapacity() { return m_vertexCapacity; };
        void ChangeVertexCapacity(unsigned int vtxCapa) { m_vertexCapacity = vtxCapa; };
        unsigned int getIndexCapacity() { return m_indexCapacity; };
        void ChangeIndexCapacity(unsigned int idxCapa) { m_indexCapacity = idxCapa; };

        class _signature_hasher
        {
        public:
            size_t operator()(const RenderBufferSignature& s) const
            {
                size_t h1 = std::hash<std::string>()((s.m_name));
                size_t h2 = std::hash<unsigned int>()(s.m_vertexCapacity + s.m_indexCapacity);
                return h1 ^ (h2 << 1);
            };
        };

    private:
        std::string m_name;
        //std::weak_ptr<Graphics::IVertexDeclaration> m_vertexDecl;
        Graphics::PrimitiveTopology m_topology;
        unsigned int m_vertexCapacity;
        unsigned int m_indexCapacity;
    };
};


#endif // RENDER_BUFFER_SIGNATURE_H
