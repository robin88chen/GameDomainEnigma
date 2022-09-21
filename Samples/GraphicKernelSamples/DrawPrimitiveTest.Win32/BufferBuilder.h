/*********************************************************************
 * \file   BufferBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef BUFFER_BUILDER_H
#define BUFFER_BUILDER_H
#include <Frameworks/Event.h>
#include <Frameworks/EventSubscriber.h>
#include <GraphicKernel/IGraphicAPI.h>

class BufferBuilder
{
public:
    BufferBuilder();
    BufferBuilder(const BufferBuilder&) = delete;
    BufferBuilder(BufferBuilder&&) = delete;
    ~BufferBuilder();

    const BufferBuilder& operator=(const BufferBuilder&) = delete;
    const BufferBuilder& operator=(BufferBuilder&&) = delete;

    void BuildVertexBuffer(const std::string& name, size_t vertex_size, const byte_buffer& data_buffer);
    void BuildIndexBuffer(const std::string& name, const uint_buffer& data_buffer);
private:
    void OnVertexBufferCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexBufferUpdated(const Enigma::Frameworks::IEventPtr& e);

    void OnIndexBufferCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnIndexBufferUpdated(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferUpdated;

    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferUpdated;

    std::string m_vtxBufferName;
    size_t m_vtxSize;
    byte_buffer m_vtxDataBuffer;
    std::string m_idxBufferName;
    uint_buffer m_idxDataBuffer;
};

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

#endif // BUFFER_BUILDER_H
