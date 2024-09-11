#include "PortalManagementNode.h"
#include "PortalManagementNodeAssembler.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "OutRegionNode.h"
#include "Culler.h"
#include "Camera.h"
#include "ContainingPortalZoneFinder.h"
#include "Platforms/PlatformLayer.h"
#include "PortalCommands.h"
#include "PortalEvents.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, PortalManagementNode, Node);

PortalManagementNode::PortalManagementNode(const SpatialId& id) : Node(id)
{
    m_outsideRegion = nullptr;
    m_factoryDesc = FactoryDesc(TYPE_RTTI.getName());
    subscribeOutRegionAttachment();
}

/*PortalManagementNode::PortalManagementNode(const SpatialId& id, const Engine::GenericDto& o) : Node(id, o)
{
    m_outsideRegion = nullptr;
    PortalManagementNodeDto dto{ o };
    if (dto.outsideRegionId()) m_outsideRegionId = dto.outsideRegionId().value();
    subscribeOutRegionAttachment();
}*/

PortalManagementNode::~PortalManagementNode()
{
    unsubscribeOutRegionAttachment();

    m_outsideRegion = nullptr;
}

std::shared_ptr<PortalManagementNode> PortalManagementNode::create(const SpatialId& id)
{
    return std::make_shared<PortalManagementNode>(id);
}

std::shared_ptr<SpatialAssembler> PortalManagementNode::assembler() const
{
    return std::make_shared<PortalManagementNodeAssembler>(m_id);
}

void PortalManagementNode::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    Node::assemble(assembler);
    if (auto portal_assembler = std::dynamic_pointer_cast<PortalManagementNodeAssembler>(assembler))
    {
        if (!m_outsideRegionId.empty()) portal_assembler->outsideRegionId(m_outsideRegionId);
    }
}

std::shared_ptr<SpatialDisassembler> PortalManagementNode::disassembler() const
{
    return std::make_shared<PortalManagementNodeDisassembler>();
}

void PortalManagementNode::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    Node::disassemble(disassembler);
    if (auto portal_disassembler = std::dynamic_pointer_cast<PortalManagementNodeDisassembler>(disassembler))
    {
        if (portal_disassembler->outsideRegionId()) m_outsideRegionId = portal_disassembler->outsideRegionId().value();
    }
}

/*std::shared_ptr<PortalManagementNode> PortalManagementNode::create(const SpatialId& id)
{
    return std::make_shared<PortalManagementNode>(id);
}

std::shared_ptr<PortalManagementNode> PortalManagementNode::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<PortalManagementNode>(id, dto);
}

GenericDto PortalManagementNode::serializeDto()
{
    PortalManagementNodeDto dto(serializeNodeDto());
    if (!m_outsideRegionId.empty()) dto.outsideRegionId(m_outsideRegionId);
    return dto.toGenericDto();
}*/

void PortalManagementNode::attachOutsideRegion(const std::shared_ptr<OutRegionNode>& node)
{
    if (!node) return;
    m_outsideRegionId = node->id();
    m_outsideRegion = node;
    node->ownerManagementNode(m_id);
    std::make_shared<OutsideRegionAttached>(m_id, m_outsideRegionId)->enqueue();
}

error PortalManagementNode::onCullingVisible(Culler* culler, bool noCull)
{
    if (FATAL_LOG_EXPR((!culler) || (!culler->GetCamera()))) return ErrorCode::nullCullerCamera;

    error er = ErrorCode::ok;
    if (!noCull)
    {
        culler->Insert(thisSpatial());
        std::shared_ptr<PortalZoneNode> startZone;
        Vector3 camPos = culler->GetCamera()->location();
        if ((m_cachedStartZone.lock()) && (m_cachedStartZone.lock()->getWorldBound().PointInside(camPos)))
        {
            startZone = m_cachedStartZone.lock();
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
        if (startZone)
        {
            er = startZone->cullVisibleSet(culler, noCull);
            if (er) return er;
        }
        else if (auto out_region = outsideRegion())
        {
            m_cachedStartZone.reset();
            er = out_region->cullVisibleSet(culler, noCull);
            if (er) return er;
        }
        else
        {  // no start zone, so cull the whole scene
            m_cachedStartZone.reset();
            er = Node::onCullingVisible(culler, noCull);
        }
    }
    else
    {
        er = Node::onCullingVisible(culler, noCull);
    }
    return er;
}

SceneTraveler::TravelResult PortalManagementNode::visitBy(SceneTraveler* traveler)
{
    SceneTraveler::TravelResult res = Node::visitBy(traveler);
    if (res != SceneTraveler::TravelResult::Continue) return res;  // don't go sub-tree
    if (auto out_region = outsideRegion())
    {
        res = m_outsideRegion->visitBy(traveler);
        if (res == SceneTraveler::TravelResult::InterruptError) return res;
        if (res == SceneTraveler::TravelResult::InterruptTargetFound) return res;
    }
    return res;
}

void PortalManagementNode::attachOutsideRegion(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachManagementOutsideRegion, Frameworks::ICommand>(c);
    if (!cmd) return;
    attachChild(cmd->getRegion(), Matrix4::IDENTITY);
    attachOutsideRegion(cmd->getRegion());
}

std::shared_ptr<OutRegionNode> PortalManagementNode::outsideRegion()
{
    if (m_outsideRegionId.empty()) return nullptr;
    if (!m_outsideRegion)
    {
        auto outside_region = std::dynamic_pointer_cast<OutRegionNode>(Node::queryNode(m_outsideRegionId));
        if (outside_region) attachOutsideRegion(outside_region);
    }
    return m_outsideRegion;
}

void PortalManagementNode::subscribeOutRegionAttachment()
{
    m_attachOutsideRegion = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { attachOutsideRegion(c); });
    Frameworks::CommandBus::subscribe(typeid(AttachManagementOutsideRegion), m_attachOutsideRegion);
}

void PortalManagementNode::unsubscribeOutRegionAttachment()
{
    Frameworks::CommandBus::unsubscribe(typeid(AttachManagementOutsideRegion), m_attachOutsideRegion);
    m_attachOutsideRegion = nullptr;
}
