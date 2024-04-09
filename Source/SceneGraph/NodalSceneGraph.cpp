#include "NodalSceneGraph.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "SceneGraphErrors.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

NodalSceneGraph::NodalSceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : SceneGraph(scene_graph_repository)
{
    m_root = nullptr;
}

NodalSceneGraph::~NodalSceneGraph()
{
    m_root = nullptr;
}

error NodalSceneGraph::createRoot(const SpatialId& scene_root_id)
{
    assert(!m_sceneGraphRepository.expired());
    assert(scene_root_id.isValid());
    m_root = Node::queryNode(scene_root_id);
    if (!m_root) return ErrorCode::sceneRepositoryFailed;
    m_sceneGraphRepository.lock()->putSpatial(m_root, PersistenceLevel::Repository);
    m_root->setLocalTransform(Matrix4::IDENTITY);
    return ErrorCode::ok;
}

error NodalSceneGraph::destroyRoot()
{
    if (!m_root) return ErrorCode::nullSceneGraph;
    m_root = nullptr;
    return ErrorCode::ok;
}

