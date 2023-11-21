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
    LazyNodeDto lazy_node_dto = LazyNodeDto::fromGenericDto(o);
    if (lazy_node_dto.isReady()) m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
}

LazyNode::~LazyNode()
{
}

GenericDto LazyNode::serializeAsLaziness()
{
    LazyNodeDto lazy_node_dto = LazyNodeDto(NodeDto(serializeSpatialDto()));
    GenericDto dto = lazy_node_dto.toGenericDto();
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
