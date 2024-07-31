#include "SceneGraphAssemblers.h"
#include "LazyNode.h"
#include "Pawn.h"
#include "SceneGraphDtos.h"
#include "SceneGraphQueries.h"
#include "Node.h"
#include "Portal.h"
#include "PortalZoneNode.h"
#include "PortalManagementNode.h"
#include "VisibilityManagedNode.h"

using namespace Enigma::SceneGraph;

PawnAssembler::PawnAssembler(const SpatialId& id) : m_spatialAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(Pawn::TYPE_RTTI.getName()));
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& PawnAssembler::spatial()
{
    return m_spatialAssembler;
}

PawnAssembler& PawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_spatialAssembler.factory(factory);
    return *this;
}

PawnAssembler& PawnAssembler::primitive(const Primitives::PrimitiveId& primitive_id)
{
    m_dto.primitiveId(primitive_id);
    return *this;
}

PawnAssembler& PawnAssembler::asNative(const std::string& file_at_path)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PawnDto PawnAssembler::toPawnDto() const
{
    PawnDto pawn_dto(m_spatialAssembler.toGenericDto());
    pawn_dto.id(m_dto.id());
    if (m_dto.primitiveId().has_value()) pawn_dto.primitiveId(m_dto.primitiveId().value());
    pawn_dto.factoryDesc(m_dto.factoryDesc());
    return pawn_dto;
}

Enigma::Engine::GenericDto PawnAssembler::toGenericDto() const
{
    return toPawnDto().toGenericDto();
}

std::shared_ptr<Pawn> PawnAssembler::constitute()
{
    return std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}

NodeAssembler::NodeAssembler(const SpatialId& id) : m_spatialAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(Node::TYPE_RTTI.getName()));
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& NodeAssembler::spatial()
{
    return m_spatialAssembler;
}

NodeAssembler& NodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_spatialAssembler.factory(factory);
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id)
{
    m_dto.pushChild({ child_id });
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const Engine::GenericDto& child_dto)
{
    m_dto.pushChild({ child_id, child_dto });
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const SpatialAssembler* child)
{
    m_dto.pushChild({ child_id });
    m_children.push_back(const_cast<SpatialAssembler*>(child));
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const NodeAssembler* child)
{
    m_dto.pushChild({ child_id });
    m_childNodes.push_back(const_cast<NodeAssembler*>(child));
    return *this;
}

NodeAssembler& NodeAssembler::asNative(const std::string& file_at_path)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

NodeDto NodeAssembler::toNodeDto() const
{
    const_cast<NodeAssembler*>(this)->consistChildrenLocationBounding();
    NodeDto node_dto(m_spatialAssembler.toGenericDto());
    node_dto.id(m_dto.id());
    node_dto.factoryDesc(m_dto.factoryDesc());
    node_dto.children(m_dto.children());
    return node_dto;
}

void NodeAssembler::consistChildrenLocationBounding()
{
    if (m_children.empty() && m_childNodes.empty()) return;
    Engine::BoundingVolume model_bound = m_spatialAssembler.modelBound();
    if (!m_childNodes.empty())
    {
        for (const auto& child : m_childNodes)
        {
            child->spatial().worldTransform(m_spatialAssembler.worldTransform() * child->spatial().localTransform());
            child->consistChildrenLocationBounding();
            model_bound.Merge(child->spatial().localTransform(), child->spatial().modelBound());
        }
    }
    if (!m_children.empty())
    {
        for (const auto& child : m_children)
        {
            child->worldTransform(m_spatialAssembler.worldTransform() * child->localTransform());
            model_bound.Merge(child->localTransform(), child->modelBound());
        }
    }
    m_spatialAssembler.modelBound(model_bound);
}

Enigma::Engine::GenericDto NodeAssembler::toGenericDto() const
{
    return toNodeDto().toGenericDto();
}

std::shared_ptr<Node> NodeAssembler::constitute()
{
    return std::dynamic_pointer_cast<Node>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}

LazyNodeAssembler::LazyNodeAssembler(const SpatialId& id) : m_nodeAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(LazyNode::TYPE_RTTI.getName()));
    m_nodeAssembler.factory(m_dto.factoryDesc());
}

NodeAssembler& LazyNodeAssembler::node()
{
    return m_nodeAssembler;
}

LazyNodeAssembler& LazyNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_nodeAssembler.factory(factory);
    return *this;
}

LazyNodeAssembler& LazyNodeAssembler::asDeferred(const std::string& filename, const std::string& path_id)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsDeferred(filename, path_id);
    m_dto.factoryDesc(fd);
    m_nodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

LazyNodeDto LazyNodeAssembler::toHydratedDto() const
{
    LazyNodeDto lazy_node_dto(m_nodeAssembler.toGenericDto());
    lazy_node_dto.id(m_dto.id());
    lazy_node_dto.factoryDesc(m_dto.factoryDesc());
    return lazy_node_dto;
}

Enigma::Engine::GenericDto LazyNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

std::shared_ptr<LazyNode> LazyNodeAssembler::constituteHydrated()
{
    return std::dynamic_pointer_cast<LazyNode>(std::make_shared<RequestSpatialConstitution>(m_id, toHydratedGenericDto())->dispatch());
}

LazyNodeDto LazyNodeAssembler::toDeHydratedDto() const
{
    LazyNodeDto lazy_node_dto(m_nodeAssembler.toGenericDto());
    lazy_node_dto.id(m_dto.id());
    lazy_node_dto.factoryDesc(m_dto.factoryDesc());
    lazy_node_dto.children({});
    return lazy_node_dto;
}

Enigma::Engine::GenericDto LazyNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}

std::shared_ptr<LazyNode> LazyNodeAssembler::constituteDeHydrated()
{
    return std::dynamic_pointer_cast<LazyNode>(std::make_shared<RequestSpatialConstitution>(m_id, toDeHydratedGenericDto())->dispatch());
}


VisibilityManagedNodeAssembler::VisibilityManagedNodeAssembler(const SpatialId& id) : m_lazyNodeAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName()));
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
}

LazyNodeAssembler& VisibilityManagedNodeAssembler::lazyNode()
{
    return m_lazyNodeAssembler;
}

VisibilityManagedNodeAssembler& VisibilityManagedNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_lazyNodeAssembler.factory(factory);
    return *this;
}

VisibilityManagedNodeAssembler& VisibilityManagedNodeAssembler::asDeferred(const std::string& file_name, const std::string& path_id)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsDeferred(file_name, path_id);
    m_dto.factoryDesc(fd);
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

VisibilityManagedNodeDto VisibilityManagedNodeAssembler::toHydratedDto() const
{
    VisibilityManagedNodeDto visi_node_dto(m_lazyNodeAssembler.toHydratedDto());
    visi_node_dto.id(m_dto.id());
    visi_node_dto.factoryDesc(m_dto.factoryDesc());
    return visi_node_dto;
}

Enigma::Engine::GenericDto VisibilityManagedNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

VisibilityManagedNodeDto VisibilityManagedNodeAssembler::toDeHydratedDto() const
{
    VisibilityManagedNodeDto visi_node_dto(m_lazyNodeAssembler.toDeHydratedDto());
    visi_node_dto.id(m_dto.id());
    visi_node_dto.factoryDesc(m_dto.factoryDesc());
    return visi_node_dto;
}

Enigma::Engine::GenericDto VisibilityManagedNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}
