#include "PortalSceneGraph.h"
#include "SceneGraphErrors.h"
#include "SceneGraphRepository.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

PortalSceneGraph::PortalSceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : SceneGraph(scene_graph_repository)
{
    m_root = nullptr;
}

PortalSceneGraph::~PortalSceneGraph()
{
    m_root = nullptr;
}

error PortalSceneGraph::createRoot(const SpatialId& scene_root_id)
{
    assert(!m_sceneGraphRepository.expired());
    assert(!scene_root_id.empty());
    m_root = std::dynamic_pointer_cast<PortalManagementNode>(Node::queryNode(scene_root_id));
    if (!m_root) return ErrorCode::sceneRepositoryFailed;
    m_root->setLocalTransform(Matrix4::IDENTITY);
    m_sceneGraphRepository.lock()->removePendingSpatialAsSceneRoot(scene_root_id);
    return ErrorCode::ok;
}

error PortalSceneGraph::destroyRoot()
{
    if (!m_root) return ErrorCode::nullSceneGraph;
    m_root = nullptr;
    return ErrorCode::ok;
}
