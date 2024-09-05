#include "PortalZoneNode.h"
#include "PortalZoneNodeAssembler.h"
#include "Portal.h"
#include "PortalManagementNode.h"
#include "SceneGraphCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphErrors.h"
#include "SceneGraphRepository.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, PortalZoneNode, LazyNode);

PortalZoneNode::PortalZoneNode(const SpatialId& id) : LazyNode(id)
{
    m_hasTraversed = false;
}

/*PortalZoneNode::PortalZoneNode(const SpatialId& id, const Engine::GenericDto& o) : LazyNode(id, o)
{
    m_hasTraversed = false;
    PortalZoneNodeDto dto{ o };
    m_portalParentId = dto.portalParentId();
}*/

PortalZoneNode::~PortalZoneNode()
{
}

std::shared_ptr<SpatialAssembler> PortalZoneNode::assembler() const
{
    return std::make_shared<DehydratedPortalZoneNodeAssembler>(m_id);
}

void PortalZoneNode::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    LazyNode::assemble(assembler);
    if (auto node_assembler = std::dynamic_pointer_cast<DehydratedPortalZoneNodeAssembler>(assembler))
    {
        if (m_portalParentId) node_assembler->portalParentId(m_portalParentId.value());
    }
}

std::shared_ptr<SpatialDisassembler> PortalZoneNode::disassembler() const
{
    return std::make_shared<DehydratedPortalZoneNodeDisassembler>();
}

void PortalZoneNode::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    LazyNode::disassemble(disassembler);
    if (auto node_disassembler = std::dynamic_pointer_cast<DehydratedPortalZoneNodeDisassembler>(disassembler))
    {
        if (node_disassembler->portalParentId()) m_portalParentId = node_disassembler->portalParentId().value();
    }
}

std::shared_ptr<HydratedLazyNodeAssembler> PortalZoneNode::assemblerOfLaziedContent() const
{
    return std::make_shared<HydratedPortalZoneNodeAssembler>(m_id);
}

std::shared_ptr<HydratedLazyNodeDisassembler> PortalZoneNode::disassemblerOfLaziedContent() const
{
    return std::make_shared<HydratedPortalZoneNodeDisassembler>();
}

void PortalZoneNode::assembleLaziedContent(const std::shared_ptr<HydratedLazyNodeAssembler>& assembler)
{
    assert(assembler);
    LazyNode::assembleLaziedContent(assembler);
    if (auto node_assembler = std::dynamic_pointer_cast<HydratedPortalZoneNodeAssembler>(assembler))
    {
        if (m_portalParentId) node_assembler->portalParentId(m_portalParentId.value());
    }
}

/*std::shared_ptr<PortalZoneNode> PortalZoneNode::create(const SpatialId& id)
{
    return std::make_shared<PortalZoneNode>(id);
}

std::shared_ptr<PortalZoneNode> PortalZoneNode::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<PortalZoneNode>(id, dto);
}

GenericDto PortalZoneNode::serializeDto()
{
    PortalZoneNodeDto dto = PortalZoneNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    if (m_portalParentId.has_value()) dto.portalParentId(m_portalParentId.value());
    return dto.toGenericDto();
}

GenericDto PortalZoneNode::serializeLaziedContent()
{
    PortalZoneNodeDto dto = PortalZoneNodeDto(LazyNodeDto(Node::serializeNodeDto()));
    if (m_portalParentId.has_value()) dto.portalParentId(m_portalParentId.value());
    return dto.toGenericDto();
}

GenericDto PortalZoneNode::serializeAsLaziness()
{
    PortalZoneNodeDto dto = PortalZoneNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    if (m_portalParentId.has_value()) dto.portalParentId(m_portalParentId.value());
    return dto.toGenericDto();
}*/

error PortalZoneNode::onCullingVisible(Culler* culler, bool noCull)
{
    // 需要讀取
    if (m_lazyStatus.isGhost())
    {
        CommandBus::enqueue(std::make_shared<HydrateLazyNode>(m_id));
        return ErrorCode::ok;
    }
    if (!m_lazyStatus.isReady())
    {
        return ErrorCode::dataNotReady;
    }
    error er = ErrorCode::ok;

    if (!m_hasTraversed)
    {
        m_hasTraversed = true;

        // Add the zone walls and contained objects.
        er = Node::onCullingVisible(culler, noCull);

        m_hasTraversed = false;
    }
    return er;
}

void PortalZoneNode::parentPortal(const SpatialId& id)
{
    m_portalParentId = id;
}

const std::optional<SpatialId>& PortalZoneNode::parentPortal() const
{
    return m_portalParentId;
}
