#include "SceneGraphDtoHelper.h"

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

CameraDtoHelper& CameraDtoHelper::Frustum(const std::string& name, Frustum::ProjectionType projection_type)
{
    m_frustumDto.Name() = name;
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
    m_cameraDto.Frustum() = m_frustumDto.ToGenericDto();
    return m_cameraDto;
}

Enigma::Engine::GenericDto CameraDtoHelper::ToGenericDto()
{
    m_cameraDto.Frustum() = m_frustumDto.ToGenericDto();
    return m_cameraDto.ToGenericDto();
}
