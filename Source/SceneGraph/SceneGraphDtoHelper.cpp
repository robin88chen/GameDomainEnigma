#include "SceneGraphDtoHelper.h"

#include "Pawn.h"
#include "SceneGraphDtos.h"

using namespace Enigma::SceneGraph;

CameraDtoHelper::CameraDtoHelper(const std::string& name)
{
    m_cameraDto.id() = SpatialId(name, Camera::TYPE_RTTI);
    m_cameraDto.HandSystem() = GraphicCoordSys::LeftHand;
}

CameraDtoHelper& CameraDtoHelper::eyePosition(const MathLib::Vector3& eye_position)
{
    m_cameraDto.EyePosition() = eye_position;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::lookAt(const MathLib::Vector3& look_at)
{
    m_cameraDto.LookAtDirection() = look_at;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::upDirection(const MathLib::Vector3& up_direction)
{
    m_cameraDto.UpVector() = up_direction;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::frustum(Frustum::ProjectionType projection_type)
{
    m_frustumDto.HandSystem() = GraphicCoordSys::LeftHand;
    m_frustumDto.ProjectionType() = projection_type;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::frustumFrontBackZ(float front_z, float back_z)
{
    m_frustumDto.NearPlaneZ() = front_z;
    m_frustumDto.FarPlaneZ() = back_z;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::frustumFov(float fov)
{
    m_frustumDto.Fov() = fov;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::frustumNearPlaneDimension(float width, float height)
{
    m_frustumDto.NearWidth() = width;
    m_frustumDto.NearHeight() = height;
    m_frustumDto.AspectRatio() = width / height;
    return *this;
}

CameraDto CameraDtoHelper::toCameraDto()
{
    m_cameraDto.Frustum() = m_frustumDto.toGenericDto();
    return m_cameraDto;
}

Enigma::Engine::GenericDto CameraDtoHelper::toGenericDto()
{
    m_cameraDto.Frustum() = m_frustumDto.toGenericDto();
    return m_cameraDto.toGenericDto();
}

SpatialDtoHelper::SpatialDtoHelper(const std::string& name)
{
    m_dto.id() = SpatialId(name, Spatial::TYPE_RTTI);
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Spatial::TYPE_RTTI.getName());
    m_dto.name() = name;
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

SpatialDtoHelper& SpatialDtoHelper::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

Enigma::Engine::GenericDto SpatialDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

PawnDtoHelper::PawnDtoHelper(const std::string& name)
{
    m_dto.id() = SpatialId(name, Pawn::TYPE_RTTI);
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
    m_dto.name() = name;
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

PawnDtoHelper& PawnDtoHelper::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    m_dto.id() = SpatialId(m_dto.name(), Frameworks::Rtti::fromName(factory.GetRttiName()));
    return *this;
}

PawnDtoHelper& PawnDtoHelper::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnDtoHelper& PawnDtoHelper::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnDtoHelper& PawnDtoHelper::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

PawnDtoHelper& PawnDtoHelper::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

PawnDtoHelper& PawnDtoHelper::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

PawnDtoHelper& PawnDtoHelper::meshPrimitive(const Renderer::MeshPrimitiveDto& mesh_dto)
{
    m_dto.primitiveId() = mesh_dto.id();
    return *this;
}

PawnDto PawnDtoHelper::toPawnDto()
{
    return m_dto;
}

Enigma::Engine::GenericDto PawnDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}
