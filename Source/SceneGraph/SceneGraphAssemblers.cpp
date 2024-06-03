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

CameraAssembler::CameraAssembler(const SpatialId& id)
{
    m_id = id;
    m_cameraDto.id() = id;
    m_cameraDto.HandSystem() = GraphicCoordSys::LeftHand;
}

CameraAssembler& CameraAssembler::eyePosition(const MathLib::Vector3& eye_position)
{
    m_cameraDto.EyePosition() = eye_position;
    return *this;
}

CameraAssembler& CameraAssembler::lookAt(const MathLib::Vector3& look_at)
{
    m_cameraDto.LookAtDirection() = look_at;
    return *this;
}

CameraAssembler& CameraAssembler::upDirection(const MathLib::Vector3& up_direction)
{
    m_cameraDto.UpVector() = up_direction;
    return *this;
}

CameraAssembler& CameraAssembler::frustum(Frustum::ProjectionType projection_type)
{
    m_frustumDto.HandSystem() = GraphicCoordSys::LeftHand;
    m_frustumDto.ProjectionType() = projection_type;
    return *this;
}

CameraAssembler& CameraAssembler::frustumFrontBackZ(float front_z, float back_z)
{
    m_frustumDto.NearPlaneZ() = front_z;
    m_frustumDto.FarPlaneZ() = back_z;
    return *this;
}

CameraAssembler& CameraAssembler::frustumFov(float fov)
{
    m_frustumDto.Fov() = fov;
    return *this;
}

CameraAssembler& CameraAssembler::frustumNearPlaneDimension(float width, float height)
{
    m_frustumDto.NearWidth() = width;
    m_frustumDto.NearHeight() = height;
    m_frustumDto.AspectRatio() = width / height;
    return *this;
}

CameraAssembler& CameraAssembler::asNative(const std::string& file_at_path)
{
    m_cameraDto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

CameraDto CameraAssembler::toCameraDto()
{
    m_cameraDto.Frustum() = m_frustumDto.toGenericDto();
    return m_cameraDto;
}

Enigma::Engine::GenericDto CameraAssembler::toGenericDto()
{
    m_cameraDto.Frustum() = m_frustumDto.toGenericDto();
    return m_cameraDto.toGenericDto();
}

std::shared_ptr<Camera> CameraAssembler::constitute()
{
    return std::make_shared<RequestCameraConstitution>(m_id, toGenericDto())->dispatch();
}

SpatialAssembler::SpatialAssembler(const SpatialId& id)
{
    m_id = id;
    m_dto.id() = id;
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Spatial::TYPE_RTTI.getName());
    //m_dto.name() = id.name();
    m_dto.isTopLevel() = false;
    m_dto.localTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.worldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    m_dto.graphDepth() = 0;
    m_dto.cullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.notifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_All);
    m_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
}

SpatialAssembler& SpatialAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    return *this;
}

SpatialAssembler& SpatialAssembler::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

SpatialAssembler& SpatialAssembler::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialAssembler& SpatialAssembler::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialAssembler& SpatialAssembler::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

SpatialAssembler& SpatialAssembler::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

SpatialAssembler& SpatialAssembler::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

SpatialAssembler& SpatialAssembler::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

SpatialAssembler& SpatialAssembler::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

Enigma::Engine::GenericDto SpatialAssembler::toGenericDto() const
{
    return m_dto.toGenericDto();
}

PawnAssembler::PawnAssembler(const SpatialId& id) : m_spatialAssembler(id)
{
    m_id = id;
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& PawnAssembler::spatial()
{
    return m_spatialAssembler;
}

PawnAssembler& PawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_spatialAssembler.factory(factory);
    return *this;
}

PawnAssembler& PawnAssembler::primitive(const Primitives::PrimitiveId& primitive_id)
{
    m_dto.primitiveId() = primitive_id;
    return *this;
}

PawnAssembler& PawnAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PawnDto PawnAssembler::toPawnDto() const
{
    PawnDto pawn_dto(m_spatialAssembler.toGenericDto());
    pawn_dto.id() = m_dto.id();
    pawn_dto.primitiveId() = m_dto.primitiveId();
    pawn_dto.factoryDesc() = m_dto.factoryDesc();
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
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(Node::TYPE_RTTI.getName());
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& NodeAssembler::spatial()
{
    return m_spatialAssembler;
}

NodeAssembler& NodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_spatialAssembler.factory(factory);
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id)
{
    m_dto.children().emplace_back(child_id);
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const Engine::GenericDto& child_dto)
{
    m_dto.children().emplace_back(child_id, child_dto);
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const SpatialAssembler* child)
{
    m_dto.children().emplace_back(child_id);
    m_children.push_back(const_cast<SpatialAssembler*>(child));
    return *this;
}

NodeAssembler& NodeAssembler::child(const SpatialId& child_id, const NodeAssembler* child)
{
    m_dto.children().emplace_back(child_id);
    m_childNodes.push_back(const_cast<NodeAssembler*>(child));
    return *this;
}

NodeAssembler& NodeAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

NodeDto NodeAssembler::toNodeDto() const
{
    const_cast<NodeAssembler*>(this)->consistChildrenLocationBounding();
    NodeDto node_dto(m_spatialAssembler.toGenericDto());
    node_dto.id() = m_dto.id();
    node_dto.factoryDesc() = m_dto.factoryDesc();
    node_dto.children() = m_dto.children();
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
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(LazyNode::TYPE_RTTI.getName());
    m_nodeAssembler.factory(m_dto.factoryDesc());
}

NodeAssembler& LazyNodeAssembler::node()
{
    return m_nodeAssembler;
}

LazyNodeAssembler& LazyNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_nodeAssembler.factory(factory);
    return *this;
}

LazyNodeAssembler& LazyNodeAssembler::asDeferred(const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsDeferred(filename, path_id);
    m_nodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

LazyNodeDto LazyNodeAssembler::toHydratedDto() const
{
    LazyNodeDto lazy_node_dto(m_nodeAssembler.toGenericDto());
    lazy_node_dto.id() = m_dto.id();
    lazy_node_dto.factoryDesc() = m_dto.factoryDesc();
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
    lazy_node_dto.id() = m_dto.id();
    lazy_node_dto.factoryDesc() = m_dto.factoryDesc();
    lazy_node_dto.children().clear();
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

PortalAssembler::PortalAssembler(const SpatialId& id) : m_spatialAssembler(id)
{
    m_id = id;
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(Portal::TYPE_RTTI.getName());
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& PortalAssembler::spatial()
{
    return m_spatialAssembler;
}

PortalAssembler& PortalAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_spatialAssembler.factory(factory);
    return *this;
}

PortalAssembler& PortalAssembler::adjacentZoneNodeId(const SpatialId& adjacent_zone_node_id)
{
    m_dto.adjacentZoneNodeId() = adjacent_zone_node_id;
    return *this;
}

PortalAssembler& PortalAssembler::isOpen(bool is_open)
{
    m_dto.isOpen() = is_open;
    return *this;
}

PortalAssembler& PortalAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalDto PortalAssembler::toPortalDto() const
{
    PortalDto portal_dto(m_spatialAssembler.toGenericDto());
    portal_dto.id() = m_dto.id();
    portal_dto.factoryDesc() = m_dto.factoryDesc();
    portal_dto.adjacentZoneNodeId() = m_dto.adjacentZoneNodeId();
    portal_dto.isOpen() = m_dto.isOpen();
    return portal_dto;
}

Enigma::Engine::GenericDto PortalAssembler::toGenericDto() const
{
    return toPortalDto().toGenericDto();
}

std::shared_ptr<Portal> PortalAssembler::constitute()
{
    return std::dynamic_pointer_cast<Portal>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}

PortalZoneNodeAssembler::PortalZoneNodeAssembler(const SpatialId& id) : m_lazyNodeAssembler(id)
{
    m_id = id;
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI.getName());
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
}

LazyNodeAssembler& PortalZoneNodeAssembler::lazyNode()
{
    return m_lazyNodeAssembler;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_lazyNodeAssembler.factory(factory);
    return *this;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::portalParentId(const SpatialId& portal_parent_id)
{
    m_dto.portalParentId() = portal_parent_id;
    return *this;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::asDeferred(const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsDeferred(filename, path_id);
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalZoneNodeDto PortalZoneNodeAssembler::toHydratedDto() const
{
    PortalZoneNodeDto portal_zone_node_dto(m_lazyNodeAssembler.toHydratedDto());
    portal_zone_node_dto.id() = m_dto.id();
    portal_zone_node_dto.factoryDesc() = m_dto.factoryDesc();
    portal_zone_node_dto.portalParentId() = m_dto.portalParentId();
    return portal_zone_node_dto;
}

Enigma::Engine::GenericDto PortalZoneNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

std::shared_ptr<PortalZoneNode> PortalZoneNodeAssembler::constituteHydrated()
{
    return std::dynamic_pointer_cast<PortalZoneNode>(std::make_shared<RequestSpatialConstitution>(m_id, toHydratedGenericDto())->dispatch());
}

PortalZoneNodeDto PortalZoneNodeAssembler::toDeHydratedDto() const
{
    PortalZoneNodeDto portal_zone_node_dto(m_lazyNodeAssembler.toDeHydratedDto());
    portal_zone_node_dto.id() = m_dto.id();
    portal_zone_node_dto.factoryDesc() = m_dto.factoryDesc();
    portal_zone_node_dto.portalParentId() = m_dto.portalParentId();
    return portal_zone_node_dto;
}

Enigma::Engine::GenericDto PortalZoneNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}

std::shared_ptr<PortalZoneNode> PortalZoneNodeAssembler::constituteDeHydrated()
{
    return std::dynamic_pointer_cast<PortalZoneNode>(std::make_shared<RequestSpatialConstitution>(m_id, toDeHydratedGenericDto())->dispatch());
}

PortalManagementNodeAssembler::PortalManagementNodeAssembler(const SpatialId& id) : m_nodeAssembler(id)
{
    m_id = id;
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(PortalManagementNode::TYPE_RTTI.getName());
    m_nodeAssembler.factory(m_dto.factoryDesc());
}

NodeAssembler& PortalManagementNodeAssembler::node()
{
    return m_nodeAssembler;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_nodeAssembler.factory(factory);
    return *this;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    m_nodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::outsideZoneNodeId(const SpatialId& outside_zone_node_id)
{
    m_dto.outsideZoneNodeId(outside_zone_node_id);
    return *this;
}

PortalManagementNodeDto PortalManagementNodeAssembler::toPortalManagementNodeDto() const
{
    PortalManagementNodeDto portal_management_node_dto(m_nodeAssembler.toNodeDto());
    portal_management_node_dto.id() = m_dto.id();
    portal_management_node_dto.factoryDesc() = m_dto.factoryDesc();
    if (m_dto.outsideZoneNodeId()) portal_management_node_dto.outsideZoneNodeId(m_dto.outsideZoneNodeId().value());
    return portal_management_node_dto;
}

Enigma::Engine::GenericDto PortalManagementNodeAssembler::toGenericDto() const
{
    return toPortalManagementNodeDto().toGenericDto();
}

VisibilityManagedNodeAssembler::VisibilityManagedNodeAssembler(const SpatialId& id) : m_lazyNodeAssembler(id)
{
    m_id = id;
    m_dto.id() = id;
    m_dto.factoryDesc() = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName());
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
}

LazyNodeAssembler& VisibilityManagedNodeAssembler::lazyNode()
{
    return m_lazyNodeAssembler;
}

VisibilityManagedNodeAssembler& VisibilityManagedNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_lazyNodeAssembler.factory(factory);
    return *this;
}

VisibilityManagedNodeAssembler& VisibilityManagedNodeAssembler::asDeferred(const std::string& file_name, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsDeferred(file_name, path_id);
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

VisibilityManagedNodeDto VisibilityManagedNodeAssembler::toHydratedDto() const
{
    VisibilityManagedNodeDto visi_node_dto(m_lazyNodeAssembler.toHydratedDto());
    visi_node_dto.id() = m_dto.id();
    visi_node_dto.factoryDesc() = m_dto.factoryDesc();
    return visi_node_dto;
}

Enigma::Engine::GenericDto VisibilityManagedNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

VisibilityManagedNodeDto VisibilityManagedNodeAssembler::toDeHydratedDto() const
{
    VisibilityManagedNodeDto visi_node_dto(m_lazyNodeAssembler.toDeHydratedDto());
    visi_node_dto.id() = m_dto.id();
    visi_node_dto.factoryDesc() = m_dto.factoryDesc();
    return visi_node_dto;
}

Enigma::Engine::GenericDto VisibilityManagedNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}
