#include "PortalZoneNode.h"
#include "PortalDtos.h"
#include "SceneGraphCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphErrors.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, PortalZoneNode, LazyNode);

PortalZoneNode::PortalZoneNode(const std::string& name) : LazyNode(name)
{
    m_factoryDesc = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName());
    m_hasTraversed = false;
}

PortalZoneNode::PortalZoneNode(const GenericDto& dto) : LazyNode(dto)
{
    m_hasTraversed = false;
}

PortalZoneNode::~PortalZoneNode()
{
}

GenericDto PortalZoneNode::serializeDto()
{
    return LazyNode::serializeDto();
}

error PortalZoneNode::onCullingVisible(Culler* culler, bool noCull)
{
    // 需要讀取
    if (m_lazyStatus.isGhost())
    {
        CommandBus::post(std::make_shared<InstanceLazyNode>(std::dynamic_pointer_cast<LazyNode, Spatial>(shared_from_this())));
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
