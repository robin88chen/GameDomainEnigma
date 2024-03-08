#include "VisibilityManagedNode.h"
#include "SceneGraphDtos.h"
#include "SceneGraphCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "Culler.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, VisibilityManagedNode, LazyNode);

VisibilityManagedNode::VisibilityManagedNode(const SpatialId& id) : LazyNode(id)
{
}

VisibilityManagedNode::VisibilityManagedNode(const SpatialId& id, const Engine::GenericDto& o) : LazyNode(id, o)
{
}

VisibilityManagedNode::VisibilityManagedNode(const std::string& name, const FactoryDesc& factory_desc)
    : LazyNode(name, factory_desc)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(factory_desc.GetRttiName(), VisibilityManagedNode::TYPE_RTTI.getName()));
}

VisibilityManagedNode::VisibilityManagedNode(const GenericDto& dto)
    : LazyNode(dto)
{
}

VisibilityManagedNode::~VisibilityManagedNode()
{

}

std::shared_ptr<VisibilityManagedNode> VisibilityManagedNode::create(const SpatialId& id)
{
    return std::make_shared<VisibilityManagedNode>(id);
}

std::shared_ptr<VisibilityManagedNode> VisibilityManagedNode::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<VisibilityManagedNode>(id, dto);
}

GenericDto VisibilityManagedNode::serializeDto()
{
    return LazyNode::serializeDto();
}

error VisibilityManagedNode::onCullingVisible(Culler* culler, bool noCull)
{
    // 需要讀取
    if (m_lazyStatus.isGhost())
    {
        CommandBus::post(std::make_shared<HydrateLazyNode>(m_id));
        return ErrorCode::ok;
    }
    if (!m_lazyStatus.isReady())
    {
        return ErrorCode::dataNotReady;
    }
    EventPublisher::post(std::make_shared<VisibilityChanged>(m_id, true));
    return Node::onCullingVisible(culler, noCull);
}

void VisibilityManagedNode::onCullingCompleteNotVisible(Culler* culler)
{
    if (!m_lazyStatus.isReady()) return;

    // let me check first
    if (!culler) return;
    if (!culler->IsOuterClippingEnable()) return;
    EventPublisher::post(std::make_shared<VisibilityChanged>(m_id, false));
}

void VisibilityManagedNode::dehydrate()
{
    while (!m_childList.empty())
    {
        detachChild(*(m_childList.begin()));
    }

    m_lazyStatus.changeStatus(LazyStatus::Status::Ghost);
}
