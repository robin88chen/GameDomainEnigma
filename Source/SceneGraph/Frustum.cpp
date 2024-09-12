#include "Frustum.h"
#include "SceneGraphRepository.h"
#include "SceneGraphErrors.h"
#include "MathLib/MathAlgorithm.h"
#include "MathLib/MathGlobal.h"
#include "Frameworks/EventPublisher.h"
#include "FrustumAssembler.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;

DEFINE_RTTI_OF_BASE(SceneGraph, Frustum);

Frustum::Frustum() : m_handCoord(GraphicCoordSys::LeftHand), m_fov(Math::PI / 4.0f), m_nearPlaneZ(0.1f), m_farPlaneZ(100.0f), m_aspectRatio(4.0f / 3.0f), m_nearWidth(40.0f), m_nearHeight(30.0f), m_projectionType(ProjectionType::Perspective)
{
    constructProjectionTransform();
}

Frustum::Frustum(GraphicCoordSys hand, ProjectionType proj)
{
    m_handCoord = hand;
    m_projectionType = proj;
    m_fov = Radian(Math::PI) / 4.0f;
    m_nearPlaneZ = 0.1f;
    m_farPlaneZ = 100.0f;
    m_aspectRatio = 4.0f / 3.0f;
    m_nearWidth = 40.0f;
    m_nearHeight = 30.0f;
    constructProjectionTransform();
}

void Frustum::constructProjectionTransform()
{
    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathAlgorithm::MakePerspectiveProjectionFovLH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathAlgorithm::MakeOrthoProjectionLH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
    else
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_mxProjTransform = MathAlgorithm::MakePerspectiveProjectionFovRH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
        }
        else
        {
            m_mxProjTransform = MathAlgorithm::MakeOrthoProjectionRH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
        }
    }
}

Frustum Frustum::fromPerspective(GraphicCoordSys hand, Radian fov, float aspect, float n_plane, float f_plane)
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

void Frustum::assemble(const std::shared_ptr<FrustumAssembler>& assembler) const
{
    assembler->projectionType(m_projectionType);
    assembler->handSystem(m_handCoord);
    assembler->frontBackZ(m_nearPlaneZ, m_farPlaneZ);
    assembler->fov(m_fov);
    assembler->nearPlaneDimension(m_nearWidth, m_nearHeight);
}

void Frustum::disassemble(const std::shared_ptr<FrustumDisassembler>& disassembler)
{
    m_projectionType = disassembler->projectionType();
    m_handCoord = disassembler->handSystem();
    m_fov = disassembler->fov();
    m_nearPlaneZ = disassembler->nearPlaneZ();
    m_farPlaneZ = disassembler->farPlaneZ();
    m_aspectRatio = disassembler->aspectRatio();
    m_nearWidth = disassembler->nearWidth();
    m_nearHeight = disassembler->nearHeight();
    constructProjectionTransform();
}

error Frustum::setPerspectiveProjection(Radian fov, float aspect, float n_plane, float f_plane)
{
    m_fov = fov;
    m_aspectRatio = aspect;
    m_nearPlaneZ = n_plane;
    m_farPlaneZ = f_plane;

    if (m_handCoord == GraphicCoordSys::LeftHand)
    {
        m_mxProjTransform = MathAlgorithm::MakePerspectiveProjectionFovLH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
    }
    else
    {
        m_mxProjTransform = MathAlgorithm::MakePerspectiveProjectionFovRH(m_fov, m_aspectRatio, m_nearPlaneZ, m_farPlaneZ);
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
        m_mxProjTransform = MathAlgorithm::MakeOrthoProjectionLH(m_nearWidth, m_nearHeight, m_nearPlaneZ, m_farPlaneZ);
    }
    else
    {
        m_mxProjTransform = MathAlgorithm::MakeOrthoProjectionRH(m_nearWidth, m_nearHeight,
            m_nearPlaneZ, m_farPlaneZ);
    }

    m_projectionType = ProjectionType::Ortho;

    return ErrorCode::ok;
}
