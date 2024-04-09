#include "SceneGraph.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphCommands.h"
#include "SceneGraphEvents.h"
#include "SceneGraphErrors.h"
#include "FindSpatialById.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;

SceneGraph::SceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository)
{
    m_sceneGraphRepository = scene_graph_repository;
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::registerHandlers()
{
    m_attachNodeChild = std::make_shared<CommandSubscriber>([=](auto c) { attachNodeChild(c); });
    CommandBus::subscribe(typeid(AttachNodeChild), m_attachNodeChild);
    m_detachNodeChild = std::make_shared<CommandSubscriber>([=](auto c) { detachNodeChild(c); });
    CommandBus::subscribe(typeid(DetachNodeChild), m_detachNodeChild);
    m_deleteSceneSpatial = std::make_shared<CommandSubscriber>([=](auto c) { deleteSceneSpatial(c); });
    CommandBus::subscribe(typeid(DeleteSceneSpatial), m_deleteSceneSpatial);
}

void SceneGraph::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(AttachNodeChild), m_attachNodeChild);
    m_attachNodeChild = nullptr;
    CommandBus::unsubscribe(typeid(DetachNodeChild), m_detachNodeChild);
    m_detachNodeChild = nullptr;
    CommandBus::unsubscribe(typeid(DeleteSceneSpatial), m_deleteSceneSpatial);
    m_deleteSceneSpatial = nullptr;
}

std::shared_ptr<Spatial> SceneGraph::findSpatial(const SpatialId& spatial_id)
{
    if (!root()) return nullptr;

    FindSpatialById finder(spatial_id);
    SceneTraveler::TravelResult res = root()->visitBy(&finder);
    if (res == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        return finder.getFoundSpatial();
    }
    return nullptr;
}

void SceneGraph::attachNodeChild(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachNodeChild>(c);
    assert(cmd);
    const auto node = std::dynamic_pointer_cast<Node>(findSpatial(cmd->nodeId()));
    if (!node)
    {
        EventPublisher::post(std::make_shared<AttachNodeChildFailed>(cmd->nodeId(), cmd->child()->id(), ErrorCode::nodeNotFound));
        return;
    }
    if (error er = node->attachChild(cmd->child(), cmd->localTransform()))
    {
        EventPublisher::post(std::make_shared<AttachNodeChildFailed>(cmd->nodeId(), cmd->child()->id(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<NodeChildAttached>(cmd->nodeId(), cmd->child()));
    }
}

void SceneGraph::detachNodeChild(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DetachNodeChild>(c);
    assert(cmd);
    const auto node = std::dynamic_pointer_cast<Node>(findSpatial(cmd->nodeId()));
    if (!node)
    {
        EventPublisher::post(std::make_shared<DetachNodeChildFailed>(cmd->nodeId(), cmd->childId(), ErrorCode::nodeNotFound));
        return;
    }
    const auto child = findSpatial(cmd->childId());
    if (!child)
    {
        EventPublisher::post(std::make_shared<DetachNodeChildFailed>(cmd->nodeId(), cmd->childId(), ErrorCode::spatialNotFound));
    }
    if (error er = node->detachChild(child))
    {
        EventPublisher::post(std::make_shared<DetachNodeChildFailed>(cmd->nodeId(), cmd->childId(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<NodeChildDetached>(cmd->nodeId(), cmd->childId()));
    }
}

void SceneGraph::deleteSceneSpatial(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DeleteSceneSpatial, ICommand>(c);
    if (!cmd) return;
    auto spatial = findSpatial(cmd->id());
    if (!spatial)
    {
        EventPublisher::post(std::make_shared<DeleteSceneSpatialFailed>(cmd->id(), ErrorCode::spatialNotFound));
        return;
    }
    if (const auto parent = spatial->getParent())
    {
        if (const auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(parent))
        {
            parent_node->detachChild(spatial);
        }
    }
    spatial = nullptr;
    EventPublisher::post(std::make_shared<SceneSpatialDeleted>(cmd->id()));
}

