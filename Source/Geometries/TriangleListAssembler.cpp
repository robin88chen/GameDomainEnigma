#include "TriangleListAssembler.h"
#include "TriangleList.h"
#include <cassert>

using namespace Enigma::Geometries;

TriangleListAssembler::TriangleListAssembler(const GeometryId& id) : GeometryAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(TriangleList::TYPE_RTTI);
    m_topology = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
}

void TriangleListAssembler::topology(Graphics::PrimitiveTopology topology)
{
    assert(topology == Graphics::PrimitiveTopology::Topology_TriangleList);
    GeometryAssembler::topology(topology);
}

TriangleListDisassembler::TriangleListDisassembler() : GeometryDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(TriangleList::TYPE_RTTI);
    m_topology = Graphics::PrimitiveTopology::Topology_TriangleList;
}

void TriangleListDisassembler::disassemble(const Engine::GenericDto& dto)
{
    GeometryDisassembler::disassemble(dto);
    assert(m_topology == Graphics::PrimitiveTopology::Topology_TriangleList);
}

