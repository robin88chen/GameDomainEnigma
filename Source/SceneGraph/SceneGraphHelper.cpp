#include "SceneGraphHelper.h"
#include "SceneGraphQueries.h"
#include "Pawn.h"

using namespace Enigma::SceneGraph;

CameraHelper::CameraHelper(const SpatialId& id) : m_id(id), m_dto(id)
{
}

CameraHelper& CameraHelper::eyePosition(const MathLib::Vector3& eye_position)
{
    m_dto.eyePosition(eye_position);
    return *this;
}

CameraHelper& CameraHelper::frustum(Frustum::ProjectionType projection_type)
{
    m_dto.frustum(projection_type);
    return *this;
}

CameraHelper& CameraHelper::frustumFov(float fov)
{
    m_dto.frustumFov(fov);
    return *this;
}

CameraHelper& CameraHelper::lookAt(const MathLib::Vector3& look_at)
{
    m_dto.lookAt(look_at);
    return *this;
}

CameraHelper& CameraHelper::frustumFrontBackZ(float front_z, float back_z)
{
    m_dto.frustumFrontBackZ(front_z, back_z);
    return *this;
}

CameraHelper& CameraHelper::frustumNearPlaneDimension(float width, float height)
{
    m_dto.frustumNearPlaneDimension(width, height);
    return *this;
}

CameraHelper& CameraHelper::upDirection(const MathLib::Vector3& up_direction)
{
    m_dto.upDirection(up_direction);
    return *this;
}

CameraHelper& CameraHelper::asNative(const std::string& file_at_path)
{
    m_dto.asNative(file_at_path);
    return *this;
}

std::shared_ptr<Camera> CameraHelper::constitute(PersistenceLevel persistence_level)
{
    return std::make_shared<RequestCameraConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch();
}

PawnHelper::PawnHelper(const SpatialId& id) : m_id(id), m_dto(id)
{
}

PawnHelper& PawnHelper::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform(local_transform);
    return *this;
}

PawnHelper& PawnHelper::topLevel(bool top_level)
{
    m_dto.topLevel(top_level);
    return *this;
}

PawnHelper& PawnHelper::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform(world_transform);
    return *this;
}

PawnHelper& PawnHelper::cullingMode(Spatial::CullingMode cull_mode)
{
    m_dto.cullingMode(cull_mode);
    return *this;
}

PawnHelper& PawnHelper::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth(graph_depth);
    return *this;
}

PawnHelper& PawnHelper::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_dto.modelBound(model_bound);
    return *this;
}

PawnHelper& PawnHelper::primitive(const Primitives::PrimitiveId& primitive_id)
{
    m_dto.primitive(primitive_id);
    return *this;
}

PawnHelper& PawnHelper::notifyFlags(Spatial::NotifyFlags notify_flags)
{
    m_dto.notifyFlags(notify_flags);
    return *this;
}

PawnHelper& PawnHelper::spatialFlags(Spatial::SpatialFlags spatial_flags)
{
    m_dto.spatialFlags(spatial_flags);
    return *this;
}

PawnHelper& PawnHelper::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factory(factory);
    return *this;
}

PawnHelper& PawnHelper::asNative(const std::string& file_at_path)
{
    m_dto.asNative(file_at_path);
    return *this;
}

std::shared_ptr<Pawn> PawnHelper::constitute(PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch());
}
