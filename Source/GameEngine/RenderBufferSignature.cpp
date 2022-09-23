#include "RenderBufferSignature.h"

Enigma::Engine::RenderBufferSignature::RenderBufferSignature()
{
    m_vertexCapacity = 0;
    m_indexCapacity = 0;
    m_topology = Graphics::PrimitiveTopology::Topology_Undefine;
}

Enigma::Engine::RenderBufferSignature::RenderBufferSignature(const std::string& name,
    Graphics::PrimitiveTopology pt, unsigned vertexCapa, unsigned idxCapa)
{
    m_name = name;
    m_topology = pt;
    m_vertexCapacity = vertexCapa;
    m_indexCapacity = idxCapa;
}

Enigma::Engine::RenderBufferSignature::RenderBufferSignature(const RenderBufferSignature& signature)
{
    m_name = signature.m_name;
    m_topology = signature.m_topology;
    m_vertexCapacity = signature.m_vertexCapacity;
    m_indexCapacity = signature.m_indexCapacity;
}

Enigma::Engine::RenderBufferSignature::RenderBufferSignature(RenderBufferSignature&& signature)
{
    m_name = std::move(signature.m_name);
    m_topology = signature.m_topology;
    m_vertexCapacity = signature.m_vertexCapacity;
    m_indexCapacity = signature.m_indexCapacity;
}

Enigma::Engine::RenderBufferSignature::~RenderBufferSignature()
{
}

Enigma::Engine::RenderBufferSignature& Enigma::Engine::RenderBufferSignature::operator=(const RenderBufferSignature& signature)
{
    m_name = signature.m_name;
    m_topology = signature.m_topology;
    m_vertexCapacity = signature.m_vertexCapacity;
    m_indexCapacity = signature.m_indexCapacity;
    return *this;
}

Enigma::Engine::RenderBufferSignature& Enigma::Engine::RenderBufferSignature::operator=(RenderBufferSignature&& signature)
{
    m_name = std::move(signature.m_name);
    m_topology = signature.m_topology;
    m_vertexCapacity = signature.m_vertexCapacity;
    m_indexCapacity = signature.m_indexCapacity;
    return *this;
}

bool Enigma::Engine::RenderBufferSignature::operator==(const RenderBufferSignature& signature) const
{
    if (m_name != signature.m_name) return false;
    if (m_vertexCapacity != signature.m_vertexCapacity) return false;
    if (m_indexCapacity != signature.m_indexCapacity) return false;
    if (m_topology != signature.m_topology) return false;
    return true;
}

bool Enigma::Engine::RenderBufferSignature::operator<(const RenderBufferSignature& signature) const
{
    if (m_name < signature.m_name) return true;
    if (m_name > signature.m_name) return false;
    if (m_vertexCapacity < signature.m_vertexCapacity) return true;
    if (m_vertexCapacity > signature.m_vertexCapacity) return false;
    if (m_indexCapacity < signature.m_indexCapacity) return true;
    if (m_indexCapacity > signature.m_indexCapacity) return false;
    if (m_topology < signature.m_topology) return true;
    if (m_topology > signature.m_topology) return false;
    // all the same
    return false;
}
