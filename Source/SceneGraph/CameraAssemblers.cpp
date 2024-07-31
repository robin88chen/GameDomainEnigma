#include "CameraAssemblers.h"
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

std::shared_ptr<Camera> CameraAssembler::constitute()
{
    return std::make_shared<RequestCameraConstitution>(m_id, toGenericDto())->dispatch();
}
