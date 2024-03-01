#include "PortalManagementNode.h"
#include "PortalDtos.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "Culler.h"
#include "Camera.h"
#include "ContainingPortalZoneFinder.h"
#include "GameEngine/LinkageResolver.h"
#include "Platforms/PlatformLayer.h"
#include "PortalCommands.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, PortalManagementNode, Node);

PortalManagementNode::PortalManagementNode(const std::string& name) : Node(name)
{
    m_factoryDesc = FactoryDesc(TYPE_RTTI.getName());
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
    m_attachOutsideZone = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { attachOutsideZone(c); });
    Frameworks::CommandBus::subscribe(typeid(AttachPortalOutsideZone), m_attachOutsideZone);
}

PortalManagementNode::PortalManagementNode(const GenericDto& dto) : Node(dto)
{
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
    m_attachOutsideZone = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { attachOutsideZone(c); });
    Frameworks::CommandBus::subscribe(typeid(AttachPortalOutsideZone), m_attachOutsideZone);
}

PortalManagementNode::~PortalManagementNode()
{
    Frameworks::CommandBus::unsubscribe(typeid(AttachPortalOutsideZone), m_attachOutsideZone);
    m_attachOutsideZone = nullptr;
    m_outsideZone = nullptr;
    m_cachedStartZone = nullptr;
}

GenericDto PortalManagementNode::serializeDto()
{
    PortalManagementNodeDto dto(serializeSpatialDto());
    if (m_outsideZone) dto.outsideZoneNodeName() = m_outsideZone->getSpatialName();
    return dto.toGenericDto();
}

void PortalManagementNode::resolveFactoryLinkage(const GenericDto& dto, FactoryLinkageResolver<Spatial>& resolver)
{
    PortalManagementNodeDto nodeDto = PortalManagementNodeDto::fromGenericDto(dto);
    resolver.tryResolveLinkage(nodeDto.outsideZoneNodeName(), [lifetime = weak_from_this()](auto sp)
        {
            if (!lifetime.expired())
                std::dynamic_pointer_cast<PortalManagementNode, Spatial>(lifetime.lock())->
                attachOutsideZone(std::dynamic_pointer_cast<PortalZoneNode, Spatial>(sp));
        });
}

void PortalManagementNode::attachOutsideZone(const std::shared_ptr<PortalZoneNode>& node)
{
    m_outsideZone = node;
    m_outsideZone->setPortalParent(shared_from_this());
}

error PortalManagementNode::onCullingVisible(Culler* culler, bool noCull)
{
    if (FATAL_LOG_EXPR((!culler) || (!culler->GetCamera()))) return ErrorCode::nullCullerCamera;

    error er = ErrorCode::ok;
    if (!noCull)
    {
        culler->Insert(thisSpatial());
        PortalZoneNodePtr startZone;
        Vector3 camPos = culler->GetCamera()->location();
        if ((m_cachedStartZone) && (m_cachedStartZone->getWorldBound().PointInside(camPos)))
        {
            startZone = m_cachedStartZone;
        }
        else
        {
            ContainingPortalZoneFinder zone_finder(camPos);
            //CSceneTraveler::TravelResult result=region_finder.travelTo(this);
            SceneTraveler::TravelResult result = visitBy(&zone_finder);
            if (result == SceneTraveler::TravelResult::InterruptError) return ErrorCode::ok;

            startZone = zone_finder.GetContainingZone();
            if (startZone) m_cachedStartZone = startZone;
        }
        if (!startZone) startZone = m_outsideZone;
        if (startZone)
        {
            er = startZone->cullVisibleSet(culler, noCull);
            if (er) return er;
        }
    }
    else
    {
        er = Node::onCullingVisible(culler, noCull);
    }
    return er;
}

void PortalManagementNode::attachOutsideZone(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachPortalOutsideZone, Frameworks::ICommand>(c);
    if (!cmd) return;
    attachChild(cmd->GetZone(), Matrix4::IDENTITY);
    attachOutsideZone(cmd->GetZone());
}
