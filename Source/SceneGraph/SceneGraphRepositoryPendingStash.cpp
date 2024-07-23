#include "SceneGraphRepositoryPendingStash.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphEvents.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

SceneGraphRepositoryPendingStash::SceneGraphRepositoryPendingStash()
{
    m_pendingSpatial.clear();
}

SceneGraphRepositoryPendingStash::~SceneGraphRepositoryPendingStash()
{
    assert(m_pendingSpatial.empty());
    m_pendingSpatial.clear();
}

void SceneGraphRepositoryPendingStash::registerHandlers()
{
    m_nodeChildAttached = std::make_shared<EventSubscriber>([=](auto e) { onNodeChildAttached(e); });
    EventPublisher::subscribe(typeid(NodeChildAttached), m_nodeChildAttached);
    m_nodeChildAttachmentFailed = std::make_shared<EventSubscriber>([=](auto e) { onNodeChildAttachmentFailed(e); });
    EventPublisher::subscribe(typeid(NodeChildAttachmentFailed), m_nodeChildAttachmentFailed);
}

void SceneGraphRepositoryPendingStash::unregisterHandlers()
{
    EventPublisher::unsubscribe(typeid(NodeChildAttached), m_nodeChildAttached);
    m_nodeChildAttached = nullptr;
    EventPublisher::unsubscribe(typeid(NodeChildAttachmentFailed), m_nodeChildAttachmentFailed);
    m_nodeChildAttachmentFailed = nullptr;
}

void SceneGraphRepositoryPendingStash::removePendingSpatialAsSceneRoot(const SpatialId& id)
{
    auto it = m_pendingSpatial.find(id);
    assert(it != m_pendingSpatial.end()); //must in pending list
    m_pendingSpatial.erase(it);
}

void SceneGraphRepositoryPendingStash::insertPendingSpatial(const SpatialId& id, const std::shared_ptr<Spatial>& spatial)
{
    auto it = m_pendingSpatial.find(id);
    assert(it == m_pendingSpatial.end()); // must new
    m_pendingSpatial[id] = spatial;
}

void SceneGraphRepositoryPendingStash::onNodeChildAttached(const IEventPtr& e)
{
    auto event = std::dynamic_pointer_cast<NodeChildAttached>(e);
    assert(event);
    if (!event) return;
    if (!event->child()) return;

    removeStashedSpatial(event->child()->id());
}

void SceneGraphRepositoryPendingStash::onNodeChildAttachmentFailed(const Frameworks::IEventPtr& e)
{
    auto event = std::dynamic_pointer_cast<NodeChildAttachmentFailed>(e);
    assert(event);
    if (!event) return;
    if (!event->childId().empty()) return;

    removeStashedSpatial(event->childId());
}

void SceneGraphRepositoryPendingStash::removeStashedSpatial(const SpatialId& id)
{
    auto it = m_pendingSpatial.find(id);
    if (it != m_pendingSpatial.end())
    {
        m_pendingSpatial.erase(it);
    }
}
