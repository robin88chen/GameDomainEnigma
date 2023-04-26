#include "LazyNode.h"
#include "SceneGraphDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LazyNode, Node);

LazyNode::LazyNode(const std::string& name) : Node(name)
{
    m_factoryDesc = Engine::FactoryDesc(LazyNode::TYPE_RTTI.GetName());
}

LazyNode::LazyNode(const GenericDto& o) : Node(o)
{
}

LazyNode::~LazyNode()
{
}

GenericDto LazyNode::SerializeDto()
{
    NodeDto node_dto(SerializeSpatialDto());  // 基本的 spatial data
    if (m_factoryDesc.GetInstanceType() == FactoryDesc::InstanceType::Native)
    {
        for (auto child : m_childList)
        {
            if (child) node_dto.ChildNames().emplace_back(child->GetSpatialName());
        }
        GenericDto dto = node_dto.ToGenericDto();
        dto.AsTopLevel(true);
        return dto;
    }
    else
    {
        return node_dto.ToGenericDto();
    }
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
