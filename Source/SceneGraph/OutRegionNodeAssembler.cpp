#include "OutRegionNodeAssembler.h"
#include "OutRegionNode.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_OWNER_MANAGEMENT_ID = "OwnerManagementID";

DehydratedOutRegionNodeAssembler::DehydratedOutRegionNodeAssembler(const SpatialId& id) : DehydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(OutRegionNode::TYPE_RTTI);
}

Enigma::Engine::GenericDto DehydratedOutRegionNodeAssembler::assemble() const
{
    Engine::GenericDto dto = DehydratedLazyNodeAssembler::assemble();
    if (m_ownerManagementId) dto.addOrUpdate(TOKEN_OWNER_MANAGEMENT_ID, m_ownerManagementId.value().tokens());
    return dto;
}

HydratedOutRegionNodeAssembler::HydratedOutRegionNodeAssembler(const SpatialId& id) : HydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(OutRegionNode::TYPE_RTTI);
}

Enigma::Engine::GenericDto HydratedOutRegionNodeAssembler::assemble() const
{
    Engine::GenericDto dto = HydratedLazyNodeAssembler::assemble();
    if (m_ownerManagementId) dto.addOrUpdate(TOKEN_OWNER_MANAGEMENT_ID, m_ownerManagementId.value().tokens());
    return dto;
}

DehydratedOutRegionNodeDisassembler::DehydratedOutRegionNodeDisassembler() : DehydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(OutRegionNode::TYPE_RTTI);
}

void DehydratedOutRegionNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    DehydratedLazyNodeDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OWNER_MANAGEMENT_ID)) m_ownerManagementId = SpatialId(v.value());
}

HydratedOutRegionNodeDisassembler::HydratedOutRegionNodeDisassembler() : HydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(OutRegionNode::TYPE_RTTI);
}

void HydratedOutRegionNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    HydratedLazyNodeDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OWNER_MANAGEMENT_ID)) m_ownerManagementId = SpatialId(v.value());
}
