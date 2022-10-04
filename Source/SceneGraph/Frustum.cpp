#include "Frustum.h"
#include "SceneGraphRepository.h"
#include "SceneGraphErrors.h"
#include "MathLib/MathAlgorithm.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::SceneGraph;

Frustum::Frustum(const std::string& name, GraphicCoordSys hand, ProjectionType proj)
{
    m_name = name;
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

Frustum::~Frustum()
{
}

error Frustum::SetPerspectiveProjection(float fov, float aspect, float n_plane, float f_plane)
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

error Frustum::SetOrthoProjection(float near_w, float near_h, float n_plane, float f_plane)
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

void Frustum::ChangeFov(float fov)
{
    if (m_projectionType == ProjectionType::Ortho) return;
    SetPerspectiveProjection(fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
}

void Frustum::ChangeNearZ(float nz)
{
    if (m_projectionType == ProjectionType::Ortho)
    {
        SetOrthoProjection(m_nearWidth, m_nearHeight, nz, m_farPlaneZ);
    }
    else
    {
        SetPerspectiveProjection(m_fov, m_aspectRatio, nz, m_farPlaneZ);
    }
}

void Frustum::ChangeFarZ(float fz)
{
    if (m_projectionType == ProjectionType::Ortho)
    {
        SetOrthoProjection(m_nearWidth, m_nearHeight, m_nearPlaneZ, fz);
    }
    else
    {
        SetPerspectiveProjection(m_fov, m_aspectRatio, m_nearPlaneZ, fz);
    }
}

void Frustum::ChangeAspectRatio(float ratio)
{
    if (m_projectionType == ProjectionType::Ortho) return;
    SetPerspectiveProjection(m_fov, ratio, m_nearPlaneZ, m_farPlaneZ);
}

void Frustum::ChangeNearHeight(float nh)
{
    if (m_projectionType == ProjectionType::Perspective) return;
    SetOrthoProjection(m_nearWidth, nh, m_nearPlaneZ, m_farPlaneZ);
}

void Frustum::ChangeNearWidth(float nw)
{
    if (m_projectionType == ProjectionType::Perspective) return;
    SetOrthoProjection(nw, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
}
