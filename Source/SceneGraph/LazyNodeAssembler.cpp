#include "LazyNodeAssembler.h"
#include "LazyNode.h"

using namespace Enigma::SceneGraph;

LazyNodeAssembler::LazyNodeAssembler(const SpatialId& id) : NodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(LazyNode::TYPE_RTTI.getName());
}

void LazyNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_factoryDesc = factory;
    if ((m_persistFilename) && (m_persistPathId)) m_factoryDesc.claimAsDeferred(m_persistFilename.value(), m_persistPathId.value());
}

void LazyNodeAssembler::persist(const std::string& filename, const std::string& path_id)
{
    m_persistFilename = filename;
    m_persistPathId = path_id;
    m_factoryDesc.claimAsDeferred(filename, path_id);
}

Enigma::Engine::GenericDto DehydratedLazyNodeAssembler::assemble() const
{
    const_cast<DehydratedLazyNodeAssembler*>(this)->consistChildrenLocationBounding(); //< still need update model bound
    return SpatialAssembler::assemble();
}

std::error_code LazyNodeDisassembler::hydrate(const std::shared_ptr<LazyNode>& node, const Engine::GenericDto& dto)
{
    assert(node);
    std::shared_ptr<HydratedLazyNodeDisassembler> disassembler = node->disassemblerOfLaziedContent();
    assert(disassembler);
    disassembler->disassemble(dto);
    return node->hydrate(disassembler);
}
