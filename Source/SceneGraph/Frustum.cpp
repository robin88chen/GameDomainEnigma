#include "Frustum.h"
#include "SceneGraphRepository.h"
#include "SceneGraphErrors.h"
#include "MathLib/MathAlgorithm.h"
#include "MathLib/MathGlobal.h"
#include "CameraFrustumDtos.h"
#include "CameraFrustumEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(SceneGraph, Frustum);

Frustum::Frustum(GraphicCoordSys hand, ProjectionType proj)
{
    m_handCoord = hand;
    m_projectionType = proj;
    m_fov = MathLib::Math::PI / 4.0f;
    m_nearPlaneZ = 0.1f;
    m_farPlaneZ = 100.0f;
    m_aspectRatio = 4.0f / 3.0f;
    m_nearWidth = 40.0f;
    m_nearHeight = 30.0f;
    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovLH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionLH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
    else
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovRH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionRH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
}

Frustum::Frustum(const GenericDto& dto)
{
    FrustumDto frustum_dto = FrustumDto::fromGenericDto(dto);
    m_handCoord = frustum_dto.HandSystem();
    m_projectionType = frustum_dto.ProjectionType();
    m_fov = frustum_dto.Fov();
    m_nearPlaneZ = frustum_dto.NearPlaneZ();
    m_farPlaneZ = frustum_dto.FarPlaneZ();
    m_aspectRatio = frustum_dto.AspectRatio();
    m_nearWidth = frustum_dto.NearWidth();
    m_nearHeight = frustum_dto.NearHeight();
    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovLH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionLH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
    else
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovRH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionRH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
}

GenericDto Frustum::serializeDto()
{
    FrustumDto dto;
    dto.HandSystem() = m_handCoord;
    dto.ProjectionType() = m_projectionType;
    dto.Fov() = m_fov;
    dto.NearPlaneZ() = m_nearPlaneZ;
    dto.FarPlaneZ() = m_farPlaneZ;
    dto.AspectRatio() = m_aspectRatio;
    dto.NearWidth() = m_nearWidth;
    dto.NearHeight() = m_nearHeight;
    return dto.toGenericDto();
}

Frustum Frustum::fromPerspective(GraphicCoordSys hand, float fov, float aspect, float n_plane, float f_plane)
{
    Frustum frustum(hand, ProjectionType::Perspective);
    frustum.setPerspectiveProjection(fov, aspect, n_plane, f_plane);
    return frustum;
}

Frustum Frustum::fromOrtho(GraphicCoordSys hand, float near_w, float near_h, float n_plane, float f_plane)
{
    Frustum frustum(hand, ProjectionType::Ortho);
    frustum.setOrthoProjection(near_w, near_h, n_plane, f_plane);
    return frustum;
}

error Frustum::setPerspectiveProjection(float fov, float aspect, float n_plane, float f_plane)
{
    m_fov = fov;
    m_aspectRatio = aspect;
    m_nearPlaneZ = n_plane;
    m_farPlaneZ = f_plane;

    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovLH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
    }
    else
    {
        m_mxProjTransform = MathLib::MathAlgorithm::MakePerspectiveProjectionFovRH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
    }

    m_projectionType = ProjectionType::Perspective;
    return ErrorCode::ok;
}

error Frustum::setOrthoProjection(float near_w, float near_h, float n_plane, float f_plane)
{
    m_nearWidth = near_w;
    m_nearHeight = near_h;
    m_nearPlaneZ = n_plane;
    m_farPlaneZ = f_plane;
    m_aspectRatio = m_nearWidth / m_nearHeight;
    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionLH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
    }
    else
    {
        m_mxProjTransform = MathLib::MathAlgorithm::MakeOrthoProjectionRH(m_nearWidth, m_nearHeight,
            m_nearPlaneZ, m_farPlaneZ);
    }

    m_projectionType = ProjectionType::Ortho;

    return ErrorCode::ok;
}
