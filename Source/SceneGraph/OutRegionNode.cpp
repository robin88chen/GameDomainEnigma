#include "OutRegionNode.h"
#include "PortalDtos.h"
#include "SceneGraphCommands.h"
#include "SceneGraphErrors.h"

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, OutRegionNode, LazyNode);

OutRegionNode::OutRegionNode(const SpatialId& id) : LazyNode(id)
{
    m_hasTraversed = false;
}

OutRegionNode::OutRegionNode(const SpatialId& id, const Engine::GenericDto& o) : LazyNode(id, o)
{
    m_hasTraversed = false;
    OutRegionNodeDto dto{ o };
    m_ownerManagementId = dto.ownerManagementId();
}

OutRegionNode::~OutRegionNode()
{
}

std::shared_ptr<OutRegionNode> OutRegionNode::create(const SpatialId& id)
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
    dto.ownerManagementId(m_ownerManagementId);
    return dto.toGenericDto();
}

Enigma::Engine::GenericDto OutRegionNode::serializeLaziedContent()
{
    OutRegionNodeDto dto = OutRegionNodeDto(LazyNodeDto(Node::serializeNodeDto()));
    dto.ownerManagementId(m_ownerManagementId);
    return dto.toGenericDto();
}

Enigma::Engine::GenericDto OutRegionNode::serializeAsLaziness()
{
    OutRegionNodeDto dto = OutRegionNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    return dto.toGenericDto();
}

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
