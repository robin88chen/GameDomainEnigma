#include "LazyNode.h"

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, LazyNode, Node);

LazyNode::LazyNode(const std::string& name) : Node(name)
{
}

LazyNode::~LazyNode()
{
}

bool LazyNode::CanVisited()
{
    return m_lazyStatus.IsReady() || m_lazyStatus.IsGhost();
}

SceneTraveler::TravelResult LazyNode::VisitBy(SceneTraveler* traveler)
{
    if (m_lazyStatus.IsGhost()) return SceneTraveler::TravelResult::Skip;
    return Node::VisitBy(traveler);
}
