/*********************************************************************
 * \file   RenderBufferEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_EVENTS_H
#define RENDER_BUFFER_EVENTS_H

#include "Frameworks/Event.h"
#include "RenderBufferSignature.h"
#include "RenderBuffer.h"
#include <string>

namespace Enigma::Engine
{
    class RenderBufferBuilt : public Frameworks::IEvent
    {
    public:
        RenderBufferBuilt(const std::string& name, const RenderBufferSignature& signature, const RenderBufferPtr& buffer) :
            m_name(name), m_signature(signature), m_buffer(buffer) {};
        const std::string& name() const { return m_name; }
        const RenderBufferSignature& signature() const { return m_signature; }
        const RenderBufferPtr& buffer() const { return m_buffer; }
    private:
        std::string m_name;
        RenderBufferSignature m_signature;
        RenderBufferPtr m_buffer;
    };
    class BuildRenderBufferFailed : public Frameworks::IEvent
    {
    public:
        BuildRenderBufferFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& name() const { return m_name; }
        std::error_code error() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // RENDER_BUFFER_EVENTS_H
