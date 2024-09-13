#include "PortalManagementNodeAssembler.h"
#include "PortalManagementNode.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_OUTSIDE_NODE_ID = "OutsideNodeID";

PortalManagementNodeAssembler::PortalManagementNodeAssembler(const SpatialId& id) : NodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(PortalManagementNode::TYPE_RTTI);
}

Enigma::Engine::GenericDto PortalManagementNodeAssembler::assemble() const
{
    Engine::GenericDto dto = NodeAssembler::assemble();
    if (m_outsideRegionId) dto.addOrUpdate(TOKEN_OUTSIDE_NODE_ID, m_outsideRegionId.value().tokens());
    return dto;
}

PortalManagementNodeDisassembler::PortalManagementNodeDisassembler() : NodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(PortalManagementNode::TYPE_RTTI);
}

void PortalManagementNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    NodeDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OUTSIDE_NODE_ID)) m_outsideRegionId = SpatialId(v.value());
}
