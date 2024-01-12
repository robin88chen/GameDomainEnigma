#include "LazyNode.h"
#include "SceneNonLazyFlattenTraversal.h"
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
    LazyNodeDto lazy_node_dto{ o };
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
Enigma::Engine::GenericDtoCollection LazyNode::serializeFlattenedTree()
{
    Engine::GenericDtoCollection collection;
    collection.push_back(serializeDto());
    collection[0].asTopLevel(true);

    SceneNonLazyFlattenTraversal flatten_children;
    for (auto& child : m_childList)
    {
        child->visitBy(&flatten_children);
    }
    if (flatten_children.GetSpatials().empty()) return collection;
    for (auto& sp : flatten_children.GetSpatials())
    {
        if ((sp->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Instanced)
            && (std::dynamic_pointer_cast<LazyNode>(sp) != nullptr))
        {
            collection.push_back(std::dynamic_pointer_cast<LazyNode>(sp)->serializeAsLaziness());
        }
        else
        {
            collection.push_back(sp->serializeDto());
        }
    }
    return collection;
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
