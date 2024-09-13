#include "PortalAssembler.h"
#include "Portal.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_ADJACENT_NODE_ID = "AdjacentNodeID";
static std::string TOKEN_IS_PORTAL_OPEN = "IsPortalOpen";

PortalAssembler::PortalAssembler(const SpatialId& id) : SpatialAssembler(id), m_isOpen(false)
{
    m_factoryDesc = Engine::FactoryDesc(Portal::TYPE_RTTI);
}

Enigma::Engine::GenericDto PortalAssembler::assemble() const
{
    Engine::GenericDto dto = SpatialAssembler::assemble();
    if (m_adjacentNodeId) dto.addOrUpdate(TOKEN_ADJACENT_NODE_ID, m_adjacentNodeId.value().tokens());
    dto.addOrUpdate(TOKEN_IS_PORTAL_OPEN, m_isOpen);
    return dto;
}

PortalDisassembler::PortalDisassembler() : SpatialDisassembler(), m_isOpen(false)
{
    m_factoryDesc = Engine::FactoryDesc(Portal::TYPE_RTTI);
}

void PortalDisassembler::disassemble(const Engine::GenericDto& dto)
{
    SpatialDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ADJACENT_NODE_ID)) m_adjacentNodeId = SpatialId(v.value());
    if (auto v = dto.tryGetValue<bool>(TOKEN_IS_PORTAL_OPEN)) m_isOpen = v.value();
}
