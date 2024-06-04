#include "PortalZoneNode.h"
#include "PortalDtos.h"
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

PortalZoneNode::PortalZoneNode(const SpatialId& id, const Engine::GenericDto& o) : LazyNode(id, o)
{
    m_hasTraversed = false;
    PortalZoneNodeDto dto{ o };
    m_portalParentId = dto.portalParentId();
}

PortalZoneNode::~PortalZoneNode()
{
}

std::shared_ptr<PortalZoneNode> PortalZoneNode::create(const SpatialId& id)
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
    dto.portalParentId(m_portalParentId);
    return dto.toGenericDto();
}

GenericDto PortalZoneNode::serializeLaziedContent()
{
    PortalZoneNodeDto dto = PortalZoneNodeDto(LazyNodeDto(Node::serializeNodeDto()));
    dto.portalParentId(m_portalParentId);
    return dto.toGenericDto();
}

GenericDto PortalZoneNode::serializeAsLaziness()
{
    PortalZoneNodeDto dto = PortalZoneNodeDto(LazyNode::serializeLazyNodeAsLaziness());
    return dto.toGenericDto();
}

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

void PortalZoneNode::setPortalParent(const SpatialId& id)
{
    m_portalParentId = id;
}
