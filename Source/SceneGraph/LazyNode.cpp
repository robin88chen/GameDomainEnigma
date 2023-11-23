#include "LazyNode.h"
#include "SceneGraphDtos.h"
#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LazyNode, Node);

LazyNode::LazyNode(const std::string& name, const FactoryDesc& factory_desc) : Node(name)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(factory_desc.GetRttiName(), LazyNode::TYPE_RTTI.getName()));
    m_factoryDesc = factory_desc;
    if (m_factoryDesc.GetInstanceType() == FactoryDesc::InstanceType::Instanced)
    {
        m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
    }
}

LazyNode::LazyNode(const GenericDto& o) : Node(o)
{
    LazyNodeDto lazy_node_dto = LazyNodeDto::fromGenericDto(o);
    if (m_factoryDesc.GetInstanceType() == FactoryDesc::InstanceType::Instanced)
    {
        m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
    }
}

LazyNode::~LazyNode()
{
}

GenericDto LazyNode::serializeAsLaziness()
{
    return serializeLazyNodeAsLaziness().toGenericDto();
}

LazyNodeDto LazyNode::serializeLazyNodeAsLaziness()
{
    LazyNodeDto lazy_node_dto = LazyNodeDto(NodeDto(serializeSpatialDto()));  // this won't serialize children, that's we want
    FactoryDesc factory_desc = m_factoryDesc;
    factory_desc.ClaimAsDeferred(); // serialize as deferred
    lazy_node_dto.factoryDesc() = factory_desc;
    return lazy_node_dto;
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
