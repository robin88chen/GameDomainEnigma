#include "LazyNode.h"
#include "SceneGraphDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LazyNode, Node);

LazyNode::LazyNode(const std::string& name) : Node(name)
{
    m_factoryDesc = Engine::FactoryDesc(LazyNode::TYPE_RTTI.getName());
}

LazyNode::LazyNode(const GenericDto& o) : Node(o)
{
}

LazyNode::~LazyNode()
{
}

/*GenericDto LazyNode::serializeDto()
{
    NodeDto node_dto(serializeSpatialDto());  // 基本的 spatial data
    if ((m_factoryDesc.GetInstanceType() == FactoryDesc::InstanceType::Native)
        || (m_factoryDesc.GetInstanceType() == FactoryDesc::InstanceType::Instanced))
    {
        for (auto child : m_childList)
        {
            if (child) node_dto.ChildNames().emplace_back(child->getSpatialName());
        }
        GenericDto dto = node_dto.ToGenericDto();
        dto.AsTopLevel(true);
        return dto;
    }
    else
    {
        return node_dto.ToGenericDto();
    }
}*/

GenericDto LazyNode::SerializeAsLaziness()
{
    LazyNodeDto lazy_node_dto = LazyNodeDto(NodeDto(serializeSpatialDto()));
    GenericDto dto = lazy_node_dto.ToGenericDto();
    FactoryDesc factory_desc = m_factoryDesc;
    factory_desc.ClaimAsDeferred(); // serialize as deferred
    dto.AddRtti(factory_desc);
    return dto;
}

bool LazyNode::canVisited()
{
    return m_lazyStatus.isReady() || m_lazyStatus.isGhost();
}

SceneTraveler::TravelResult LazyNode::visitBy(SceneTraveler* traveler)
{
    if (m_lazyStatus.isGhost()) return SceneTraveler::TravelResult::Skip;
    return Node::visitBy(traveler);
}
