#include "SceneGraphAssemblers.h"
#include "Pawn.h"
#include "SceneGraphDtos.h"
#include "SceneGraphQueries.h"

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

std::shared_ptr<Camera> CameraAssembler::constitute(PersistenceLevel persistence_level)
{
    return std::make_shared<RequestCameraConstitution>(m_id, toGenericDto(), persistence_level)->dispatch();
}

SpatialAssembler::SpatialAssembler(const SpatialId& id)
{
    m_id = id;
    m_dto.id() = id;
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Spatial::TYPE_RTTI.getName());
    m_dto.name() = id.name();
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

Enigma::Engine::GenericDto SpatialAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

PawnAssembler::PawnAssembler(const SpatialId& id)
{
    m_id = id;
    m_dto.id() = id;
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
    m_dto.name() = id.name();
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

PawnAssembler& PawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_dto.id() = SpatialId(m_dto.name(), Frameworks::Rtti::fromName(factory.GetRttiName()));
    return *this;
}

PawnAssembler& PawnAssembler::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

PawnAssembler& PawnAssembler::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnAssembler& PawnAssembler::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnAssembler& PawnAssembler::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

PawnAssembler& PawnAssembler::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

PawnAssembler& PawnAssembler::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

PawnAssembler& PawnAssembler::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

PawnAssembler& PawnAssembler::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
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
    return *this;
}

PawnDto PawnAssembler::toPawnDto()
{
    return m_dto;
}

Enigma::Engine::GenericDto PawnAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

std::shared_ptr<Pawn> PawnAssembler::constitute(PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto(), persistence_level)->dispatch());
}
