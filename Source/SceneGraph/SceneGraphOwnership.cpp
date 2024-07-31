#include "SceneGraphOwnership.h"
#include "Spatial.h"
#include "SpatialId.h"
#include "PortalZoneNode.h"
#include "OutRegionNode.h"
#include "Portal.h"
#include "PortalManagementNode.h"
#include "Node.h"
#include "SceneGraphQueries.h"
#include "SceneGraphErrors.h"

using namespace Enigma::SceneGraph;

const std::optional<SpatialId>& Ownership::getAssignedOwner(const std::shared_ptr<Spatial>& owned)
{
    assert(owned);
    if (owned->id().rtti().isDerived(PortalZoneNode::TYPE_RTTI))
    {
        return std::dynamic_pointer_cast<PortalZoneNode>(owned)->parentPortal();
    }
    else if (owned->id().rtti().isDerived(OutRegionNode::TYPE_RTTI))
    {
        return std::dynamic_pointer_cast<OutRegionNode>(owned)->ownerManagementNode();
    }
    else
    {
        return owned->getParentId();
    }
}

error Ownership::attachOwnership(const SpatialId& owner, const std::shared_ptr<Spatial>& owned)
{
    assert(owned);
    if (owned->id().rtti().isDerived(PortalZoneNode::TYPE_RTTI))
    {
        return attachOwnershipOfPortalZone(owner, owned);
    }
    else if (owned->id().rtti().isDerived(OutRegionNode::TYPE_RTTI))
    {
        return attachOwnershipOfOutRegion(owner, owned);
    }
    else
    {
        return  attachOwnershipOfSpatial(owner, owned);
    }
}

error Ownership::attachOwnershipOfPortalZone(const SpatialId& owner, const std::shared_ptr<Spatial>& owned)
{
    assert(owned->id().rtti().isDerived(PortalZoneNode::TYPE_RTTI));
    assert(owner.rtti().isDerived(Portal::TYPE_RTTI));
    std::shared_ptr<Portal> portal = std::dynamic_pointer_cast<Portal>(std::make_shared<QueryRunningSpatial>(owner)->dispatch());
    if (!portal) return ErrorCode::runningSpatialNotFound;
    portal->adjacentZone(std::dynamic_pointer_cast<PortalZoneNode>(owned));
    return ErrorCode::ok;
}

error Ownership::attachOwnershipOfOutRegion(const SpatialId& owner, const std::shared_ptr<Spatial>& owned)
{
    assert(owned->id().rtti().isDerived(OutRegionNode::TYPE_RTTI));
    assert(owner.rtti().isDerived(PortalManagementNode::TYPE_RTTI));
    std::shared_ptr<PortalManagementNode> portal_management_node = std::dynamic_pointer_cast<PortalManagementNode>(std::make_shared<QueryRunningSpatial>(owner)->dispatch());
    if (!portal_management_node) return ErrorCode::runningSpatialNotFound;
    portal_management_node->attachOutsideRegion(std::dynamic_pointer_cast<OutRegionNode>(owned));
    return ErrorCode::ok;
}

error Ownership::attachOwnershipOfSpatial(const SpatialId& owner, const std::shared_ptr<Spatial>& owned)
{
    assert(owner.rtti().isDerived(Node::TYPE_RTTI));
    std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(std::make_shared<QueryRunningSpatial>(owner)->dispatch());
    if (!node) return ErrorCode::runningSpatialNotFound;
    node->attachChild(owned, owned->getLocalTransform());
    return ErrorCode::ok;
}