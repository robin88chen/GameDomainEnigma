#include "SceneGraphDtoHelper.h"

#include "Pawn.h"
#include "SceneGraphDtos.h"

using namespace Enigma::SceneGraph;

CameraDtoHelper::CameraDtoHelper(const std::string& name)
{
    m_cameraDto.Name() = name;
    m_cameraDto.HandSystem() = GraphicCoordSys::LeftHand;
}

CameraDtoHelper& CameraDtoHelper::EyePosition(const MathLib::Vector3& eye_position)
{
    m_cameraDto.EyePosition() = eye_position;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::LookAt(const MathLib::Vector3& look_at)
{
    m_cameraDto.LookAtDirection() = look_at;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::UpDirection(const MathLib::Vector3& up_direction)
{
    m_cameraDto.UpVector() = up_direction;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::Frustum(Frustum::ProjectionType projection_type)
{
    m_frustumDto.HandSystem() = GraphicCoordSys::LeftHand;
    m_frustumDto.ProjectionType() = projection_type;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::FrustumFrontBackZ(float front_z, float back_z)
{
    m_frustumDto.NearPlaneZ() = front_z;
    m_frustumDto.FarPlaneZ() = back_z;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::FrustumFov(float fov)
{
    m_frustumDto.Fov() = fov;
    return *this;
}

CameraDtoHelper& CameraDtoHelper::FrustumNearPlaneDimension(float width, float height)
{
    m_frustumDto.NearWidth() = width;
    m_frustumDto.NearHeight() = height;
    m_frustumDto.AspectRatio() = width / height;
    return *this;
}

CameraDto CameraDtoHelper::ToCameraDto()
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
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Spatial::TYPE_RTTI.getName());
    m_dto.Name() = name;
    m_dto.IsTopLevel() = false;
    m_dto.LocalTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.WorldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.ModelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    m_dto.GraphDepth() = 0;
    m_dto.CullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.NotifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_All);
    m_dto.SpatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
}

SpatialDtoHelper& SpatialDtoHelper::LocalTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.LocalTransform() = local_transform;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::WorldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.WorldTransform() = world_transform;
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::ModelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.ModelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::GraphDepth(unsigned graph_depth)
{
    m_dto.GraphDepth() = graph_depth;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::CullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.CullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::NotifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.NotifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::TopLevel(bool top_level)
{
    m_dto.IsTopLevel() = top_level;
    return *this;
}

SpatialDtoHelper& SpatialDtoHelper::SpatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.SpatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

Enigma::Engine::GenericDto SpatialDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

PawnDtoHelper::PawnDtoHelper(const std::string& name)
{
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
    m_dto.Name() = name;
    m_dto.IsTopLevel() = false;
    m_dto.LocalTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.WorldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.ModelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    m_dto.GraphDepth() = 0;
    m_dto.CullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.NotifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_All);
    m_dto.SpatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
}

PawnDtoHelper& PawnDtoHelper::Factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::LocalTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.LocalTransform() = local_transform;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::WorldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.WorldTransform() = world_transform;
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnDtoHelper& PawnDtoHelper::ModelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.ModelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).serializeDto().toGenericDto();
    return *this;
}

PawnDtoHelper& PawnDtoHelper::GraphDepth(unsigned graph_depth)
{
    m_dto.GraphDepth() = graph_depth;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::CullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.CullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

PawnDtoHelper& PawnDtoHelper::NotifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.NotifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

PawnDtoHelper& PawnDtoHelper::TopLevel(bool top_level)
{
    m_dto.IsTopLevel() = top_level;
    return *this;
}

PawnDtoHelper& PawnDtoHelper::SpatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.SpatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

PawnDtoHelper& PawnDtoHelper::MeshPrimitive(const Renderer::MeshPrimitiveDto& mesh_dto)
{
    m_dto.ThePrimitive() = mesh_dto.toGenericDto();
    return *this;
}

PawnDto PawnDtoHelper::ToPawnDto()
{
    return m_dto;
}

Enigma::Engine::GenericDto PawnDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}
