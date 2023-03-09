#include "PortalManagementNode.h"
#include "PortalDtos.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "GameEngine/LinkageResolver.h"

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, PortalManagementNode, Node);

PortalManagementNode::PortalManagementNode(const std::string& name) : Node(name)
{
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
}

PortalManagementNode::PortalManagementNode(const PortalManagementNodeDto& dto) : Node(dynamic_cast<const NodeDto&>(dto))
{
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
}

PortalManagementNode::~PortalManagementNode()
{
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
}

Enigma::Engine::GenericDto PortalManagementNode::SerializeDto()
{
    PortalManagementNodeDto dto(SerializeSpatialDto());
    if (m_outsideZone) dto.OutsideZoneNodeName() = m_outsideZone->GetSpatialName();
    return dto.ToGenericDto();
}

void PortalManagementNode::ResolveFactoryLinkage(const std::string& outside_node_name, Engine::FactoryLinkageResolver<Spatial>& resolver)
{
    resolver.TryResolveLinkage(outside_node_name, [lifetime = weak_from_this()](auto sp)
        {
            if (!lifetime.expired())
                std::dynamic_pointer_cast<PortalManagementNode, Spatial>(lifetime.lock())->
                    AttachOutsideZone(std::dynamic_pointer_cast<PortalZoneNode, Spatial>(sp));
        });
}

void PortalManagementNode::AttachOutsideZone(const std::shared_ptr<PortalZoneNode>& node)
{
    m_outsideZone = node;
}

error PortalManagementNode::OnCullingVisible(Culler* culler, bool noCull)
{
    return ErrorCode::ok;
}
