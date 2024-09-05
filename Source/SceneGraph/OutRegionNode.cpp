#include "OutRegionNode.h"
#include "OutRegionNodeAssembler.h"
#include "SceneGraphCommands.h"
#include "SceneGraphErrors.h"

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, OutRegionNode, LazyNode);

OutRegionNode::OutRegionNode(const SpatialId& id) : LazyNode(id)
{
    m_hasTraversed = false;
}

/*OutRegionNode::OutRegionNode(const SpatialId& id, const Engine::GenericDto& o) : LazyNode(id, o)
{
    m_hasTraversed = false;
    OutRegionNodeDto dto{ o };
    m_ownerManagementId = dto.ownerManagementId();
}*/

OutRegionNode::~OutRegionNode()
{
}

std::shared_ptr<SpatialAssembler> OutRegionNode::assembler() const
{
    return std::make_shared<DehydratedOutRegionNodeAssembler>(m_id);
}

void OutRegionNode::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    LazyNode::assemble(assembler);
    if (auto outRegionAssembler = std::dynamic_pointer_cast<DehydratedOutRegionNodeAssembler>(assembler))
    {
        if (m_ownerManagementId.has_value()) outRegionAssembler->ownerManagementId(m_ownerManagementId.value());
    }
}

std::shared_ptr<SpatialDisassembler> OutRegionNode::disassembler() const
{
    return std::make_shared<DehydratedOutRegionNodeDisassembler>();
}

void OutRegionNode::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    LazyNode::disassemble(disassembler);
    if (auto outRegionDisassembler = std::dynamic_pointer_cast<DehydratedOutRegionNodeDisassembler>(disassembler))
    {
        if (outRegionDisassembler->ownerManagementId()) m_ownerManagementId = outRegionDisassembler->ownerManagementId().value();
    }
}

std::shared_ptr<HydratedLazyNodeAssembler> OutRegionNode::assemblerOfLaziedContent() const
{
    return std::make_shared<HydratedOutRegionNodeAssembler>(m_id);
}

std::shared_ptr<HydratedLazyNodeDisassembler> OutRegionNode::disassemblerOfLaziedContent() const
{
    return std::make_shared<HydratedOutRegionNodeDisassembler>();
}

void OutRegionNode::assembleLaziedContent(const std::shared_ptr<HydratedLazyNodeAssembler>& assembler)
{
    assert(assembler);
    LazyNode::assembleLaziedContent(assembler);
    if (auto outRegionAssembler = std::dynamic_pointer_cast<HydratedOutRegionNodeAssembler>(assembler))
    {
        if (m_ownerManagementId.has_value()) outRegionAssembler->ownerManagementId(m_ownerManagementId.value());
    }
}

/*std::shared_ptr<OutRegionNode> OutRegionNode::create(const SpatialId& id)
{
    return std::make_shared<OutRegionNode>(id);
}

std::shared_ptr<OutRegionNode> OutRegionNode::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<OutRegionNode>(id, dto);
}

Enigma::Engine::GenericDto OutRegionNode::serializeDto()
{
    OutRegionNodeDto dto = OutRegionNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    if (m_ownerManagementId.has_value()) dto.ownerManagementId(m_ownerManagementId.value());
    return dto.toGenericDto();
}

Enigma::Engine::GenericDto OutRegionNode::serializeLaziedContent()
{
    OutRegionNodeDto dto = OutRegionNodeDto(LazyNodeDto(Node::serializeNodeDto()));
    if (m_ownerManagementId.has_value()) dto.ownerManagementId(m_ownerManagementId.value());
    return dto.toGenericDto();
}

Enigma::Engine::GenericDto OutRegionNode::serializeAsLaziness()
{
    OutRegionNodeDto dto = OutRegionNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    if (m_ownerManagementId.has_value()) dto.ownerManagementId(m_ownerManagementId.value());
    return dto.toGenericDto();
}*/

error OutRegionNode::onCullingVisible(Culler* culler, bool noCull)
{
    // 需要讀取
    if (m_lazyStatus.isGhost())
    {
        std::make_shared<HydrateLazyNode>(m_id)->enqueue();
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

void OutRegionNode::ownerManagementNode(const SpatialId& id)
{
    m_ownerManagementId = id;
}

const std::optional<SpatialId>& OutRegionNode::ownerManagementNode() const
{
    return m_ownerManagementId;
}
