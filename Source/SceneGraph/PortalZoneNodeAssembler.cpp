#include "PortalZoneNodeAssembler.h"
#include "PortalZoneNode.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_PORTAL_PARENT_ID = "PortalParentID";

DehydratedPortalZoneNodeAssembler::DehydratedPortalZoneNodeAssembler(const SpatialId& id) : DehydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI);
}

Enigma::Engine::GenericDto DehydratedPortalZoneNodeAssembler::assemble() const
{
    Engine::GenericDto dto = DehydratedLazyNodeAssembler::assemble();
    if (m_portalParentId) dto.addOrUpdate(TOKEN_PORTAL_PARENT_ID, m_portalParentId.value().tokens());
    return dto;
}

HydratedPortalZoneNodeAssembler::HydratedPortalZoneNodeAssembler(const SpatialId& id) : HydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI);
}

Enigma::Engine::GenericDto HydratedPortalZoneNodeAssembler::assemble() const
{
    Engine::GenericDto dto = HydratedLazyNodeAssembler::assemble();
    if (m_portalParentId) dto.addOrUpdate(TOKEN_PORTAL_PARENT_ID, m_portalParentId.value().tokens());
    return dto;
}

DehydratedPortalZoneNodeDisassembler::DehydratedPortalZoneNodeDisassembler() : DehydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI);
}

void DehydratedPortalZoneNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    DehydratedLazyNodeDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PORTAL_PARENT_ID)) m_portalParentId = SpatialId(v.value());
}

HydratedPortalZoneNodeDisassembler::HydratedPortalZoneNodeDisassembler() : HydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI);
}

void HydratedPortalZoneNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    HydratedLazyNodeDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PORTAL_PARENT_ID)) m_portalParentId = SpatialId(v.value());
}
