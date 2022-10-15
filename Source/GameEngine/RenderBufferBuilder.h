/*********************************************************************
 * \file   RenderBufferBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_BUILDER_H
#define RENDER_BUFFER_BUILDER_H

#include "RenderBufferBuildingPolicies.h"
#include "RenderBuffer.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GraphicKernel/IVertexBuffer.h"
#include "GraphicKernel/IIndexBuffer.h"

namespace Enigma::Engine
{
    class RenderBufferRepository;

    class RenderBufferBuilder
    {
    private:
        class VertexBufferBuilt : public Enigma::Frameworks::IEvent
        {
        public:
            VertexBufferBuilt(const std::string& name) :
                m_name(name) {};
            const std::string& GetBufferName() { return m_name; }
        private:
            std::string m_name;
        };
        class IndexBufferBuilt : public Enigma::Frameworks::IEvent
        {
        public:
            IndexBufferBuilt(const std::string& name) :
                m_name(name) {};
            const std::string& GetBufferName() { return m_name; }
        private:
            std::string m_name;
        };
    public:
        class RenderBufferBuilt : public Frameworks::IEvent
        {
        public:
            RenderBufferBuilt(const std::string& name, const RenderBufferSignature& signature, const RenderBufferPtr& buffer) :
                m_name(name), m_signature(signature), m_buffer(buffer) {};
            const std::string& GetName() const { return m_name; }
            const RenderBufferSignature& GetSignature() const { return m_signature; }
            const RenderBufferPtr& GetBuffer() const { return m_buffer; }
        private:
            std::string m_name;
            RenderBufferSignature m_signature;
            RenderBufferPtr m_buffer;
        };
    public:
        RenderBufferBuilder(RenderBufferRepository* host);
        RenderBufferBuilder(const RenderBufferBuilder&) = delete;
        RenderBufferBuilder(RenderBufferBuilder&&) = delete;
        ~RenderBufferBuilder();
        RenderBufferBuilder& operator=(const RenderBufferBuilder&) = delete;
        RenderBufferBuilder& operator=(RenderBufferBuilder&&) = delete;

        void BuildRenderBuffer(const RenderBufferPolicy& policy);
    private:
        void OnVertexBufferCreated(const Frameworks::IEventPtr& e);
        void OnVertexBufferUpdated(const Frameworks::IEventPtr& e);
        void OnIndexBufferCreated(const Frameworks::IEventPtr& e);
        void OnIndexBufferUpdated(const Frameworks::IEventPtr& e);

        void OnBufferUpdateFailed(const Frameworks::IEventPtr& e);
        void OnBufferBuilt(const Frameworks::IEventPtr& e);

    private:
        Frameworks::EventSubscriberPtr m_onVertexBufferCreated;
        Frameworks::EventSubscriberPtr m_onVertexBufferUpdated;
        Frameworks::EventSubscriberPtr m_onIndexBufferCreated;
        Frameworks::EventSubscriberPtr m_onIndexBufferUpdated;

        Frameworks::EventSubscriberPtr m_onVertexBufferUpdateFailed;
        Frameworks::EventSubscriberPtr m_onIndexBufferUpdateFailed;
        Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
        Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;

        RenderBufferRepository* m_hostRepository;

        Graphics::IVertexBufferPtr m_vertexBuffer;
        Graphics::IIndexBufferPtr m_indexBuffer;

        RenderBufferPolicy m_policy;
    };
}

#endif // RENDER_BUFFER_BUILDER_H
