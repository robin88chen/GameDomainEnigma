#include "SceneGraphCommands.h"
#include "LazyNode.h"

using namespace Enigma::SceneGraph;

HydrateLazyNode::HydrateLazyNode(const SpatialId& id) : m_id(id)
{
    auto lazy_node = std::dynamic_pointer_cast<LazyNode>(Node::queryNode(id));
    assert(lazy_node);
    if (lazy_node->lazyStatus().isGhost()) lazy_node->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::InQueue);
}
