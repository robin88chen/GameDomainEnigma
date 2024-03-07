#include "LazyNode.h"
#include "SceneNonLazyFlattenTraversal.h"
#include "SceneGraphDtos.h"
#include "SceneGraphErrors.h"
#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"
#include "SceneGraphQueries.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LazyNode, Node);

LazyNode::LazyNode(const SpatialId& id) : Node(id)
{
    m_factoryDesc = FactoryDesc(LazyNode::TYPE_RTTI.getName());
    m_factoryDesc.ClaimAsDeferred();
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
}

LazyNode::LazyNode(const SpatialId& id, const Engine::GenericDto& o) : Node(id, o)
{
    LazyNodeDto lazy_node_dto{ o };
    m_factoryDesc = lazy_node_dto.factoryDesc();
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
}

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

std::shared_ptr<LazyNode> LazyNode::create(const SpatialId& id)
{
    return std::make_shared<LazyNode>(id);
}

std::shared_ptr<LazyNode> LazyNode::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<LazyNode>(id, dto);
}

std::error_code LazyNode::hydrate(const Engine::GenericDto& dto)
{
    LazyNodeDto lazy_node_dto{ dto };
    for (auto& child : lazy_node_dto.children())
    {
        auto child_spatial = std::make_shared<QuerySpatial>(child.id())->dispatch();
        if (!child_spatial)
        {
            if (!child.dto().has_value()) return ErrorCode::childDtoNotFound;
            child_spatial = std::make_shared<RequestSpatialConstitution>(child.id(), child.dto().value(), PersistenceLevel::Repository)->dispatch();
        }
        if (child_spatial)
        {
            auto er = attachChild(child_spatial, child_spatial->getLocalTransform());
            if (er)
            {
                m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Failed);
                return er;
            }
        }
    }
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
    return ErrorCode::ok;
}

GenericDto LazyNode::serializeDto()
{
    return serializeAsLaziness();
}

GenericDto LazyNode::serializeLaziedContent()
{
    return serializeNodeDto().toGenericDto();
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
