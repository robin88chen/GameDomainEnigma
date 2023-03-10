#include "PortalManagementNode.h"
#include "PortalDtos.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "Culler.h"
#include "Camera.h"
#include "ContainingPortalZoneFinder.h"
#include "GameEngine/LinkageResolver.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

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
    if (FATAL_LOG_EXPR((!culler) || (!culler->GetCamera()))) return ErrorCode::nullCullerCamera;

    error er = ErrorCode::ok;
    if (!noCull)
    {
        culler->Insert(ThisSpatial());
        PortalZoneNodePtr startZone;
        Vector3 camPos = culler->GetCamera()->GetLocation();
        if ((m_cachedStartZone) && (m_cachedStartZone->GetWorldBound().PointInside(camPos)))
        {
            startZone = m_cachedStartZone;
        }
        else
        {
            ContainingPortalZoneFinder zone_finder(camPos);
            //CSceneTraveler::TravelResult result=region_finder.TravelTo(this);
            SceneTraveler::TravelResult result = VisitBy(&zone_finder);
            if (result == SceneTraveler::TravelResult::InterruptError) return ErrorCode::ok;

            startZone = zone_finder.GetContainingZone();
            if (startZone) m_cachedStartZone = startZone;
        }
        if (!startZone) startZone = m_outsideZone;
        if (startZone)
        {
            er = startZone->CullVisibleSet(culler, noCull);
            if (er) return er;
        }
    }
    else
    {
        er = Node::OnCullingVisible(culler, noCull);
    }
    return er;
}
