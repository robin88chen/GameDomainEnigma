#include "Camera.h"
#include "SceneGraphErrors.h"
#include "Frustum.h"
#include "CameraFrustumEvents.h"
#include "MathLib/Ray3.h"
#include "MathLib/IntrRay3Plane3.h"
#include "MathLib/MathGlobal.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphQueries.h"
#include "MathLib/Quaternion.h"
#include "CameraAssembler.h"
#include <cassert>
#include <memory>

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(SceneGraph, Camera);

Camera::Camera(const SpatialId& id) : Camera(id, GraphicCoordSys::LeftHand)
{
}

Camera::Camera(const SpatialId& id, GraphicCoordSys hand) : m_id(id), m_factoryDesc(TYPE_RTTI.getName())
{
    m_handSys = hand;
    m_cullingFrustum = Frustum::fromPerspective(hand, Radian(Math::PI / 4.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    m_vecLocation = Vector3::ZERO;
    m_vecEyeToLookAt = Vector3::UNIT_Z;
    m_vecUp = Vector3::UNIT_Y;
    m_vecRight = Vector3::UNIT_X;
}

Camera::~Camera()
{
}

std::shared_ptr<Camera> Camera::queryCamera(const SpatialId& id)
{
    assert(id.rtti().isDerived(Camera::TYPE_RTTI));
    return std::make_shared<QueryCamera>(id)->dispatch();
}

void Camera::assemble(const std::shared_ptr<CameraAssembler>& assembler) const
{
    assembler->factoryDesc(m_factoryDesc);
    assembler->handSystem(m_handSys);
    assembler->eyePosition(m_vecLocation);
    assembler->lookAt(m_vecEyeToLookAt);
    assembler->upDirection(m_vecUp);
    assembler->frustum(m_cullingFrustum);
}

void Camera::disassemble(const std::shared_ptr<CameraDisassembler>& disassembler)
{
    m_factoryDesc = disassembler->factoryDesc();
    m_handSys = disassembler->handSystem();
    changeCameraFrame(disassembler->eyePosition(), disassembler->lookAt(), disassembler->upDirection());
    m_cullingFrustum = disassembler->frustum();
}

error Camera::changeCameraFrame(const std::optional<Vector3>& eye,
    const std::optional<Vector3>& eye_to_lookat, const std::optional<Vector3>& up)
{
    if (eye)
    {
        m_vecLocation = eye.value();
    }
    assert(eye_to_lookat.has_value() == up.has_value());  // must be both set or both not set
    if (eye_to_lookat)
    {
        Vector3 _eye_to_lookat = eye_to_lookat.value().normalize();
        m_vecEyeToLookAt = _eye_to_lookat;
        if (m_handSys == GraphicCoordSys::LeftHand)
        {
            m_vecCameraForward = _eye_to_lookat;
        }
        else
        {
            m_vecCameraForward = -_eye_to_lookat;
        }
    }
    if (up)
    {
        Vector3 _up = up.value().normalize();
        Vector3 _right = _up.cross(m_vecCameraForward);
        m_vecRight = _right.normalize();
        m_vecUp = m_vecCameraForward.cross(m_vecRight);
    }

    _updateViewTransform();
    return ErrorCode::ok;
}

error Camera::zoom(float dist)
{
    error er = changeCameraFrame(m_vecLocation + dist * m_vecEyeToLookAt, std::nullopt, std::nullopt);
    if (!er) EventPublisher::enqueue(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::sphereRotate(float horz_angle, float vert_angle, const Vector3& center)
{
    Vector3 vecCenter = center;
    if (center == Vector3::ZERO)
    {
        Vector3 eye = m_vecLocation;
        Vector3 dir = m_vecEyeToLookAt;
        if (std::fabs(dir.y()) > Math::Epsilon())
        {
            float t = (-eye.y()) / dir.y();
            vecCenter = eye + t * dir;
        }
    }
    // thx to george lu
    Vector3 right = m_vecRight;
    right = Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * right;

    Vector3 dir = Quaternion(right, vert_angle) *
        Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * m_vecEyeToLookAt;    // 對XZ軸做旋轉
    Vector3 up = Quaternion(right, vert_angle) *
        Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * m_vecUp; // 對XZ軸做旋轉
    float radius;
    Vector3 dist = m_vecLocation - vecCenter;
    radius = dist.length();
    Vector3 loc = vecCenter - radius * dir;  // 新的camera位置

    error er = changeCameraFrame(loc, dir, up);
    if (!er) EventPublisher::enqueue(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::move(float dir_dist, float slide_dist)
{
    Vector3 right = m_vecRight;
    Vector3 up = Vector3::UNIT_Y;

    Vector3 move_dir = right.cross(up);
    Vector3 move_right = up.cross(move_dir);
    Vector3 pos = m_vecLocation + dir_dist * move_dir + slide_dist * move_right;

    error er = changeCameraFrame(pos, std::nullopt, std::nullopt);
    if (!er) EventPublisher::enqueue(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::moveXZ(float move_x, float move_z)
{
    error er = changeCameraFrame(m_vecLocation + Vector3(move_x, 0.0f, move_z), std::nullopt, std::nullopt);
    if (!er) EventPublisher::enqueue(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::shiftLookAt(const Vector3& vecLookAt)
{
    Vector3 dir = m_vecEyeToLookAt;
    Vector3 loc = m_vecLocation;
    Plane3 horz_plane(Vector3::UNIT_Y, loc);
    Ray3 dir_ray(vecLookAt, -dir);
    IntrRay3Plane3 intr(dir_ray, horz_plane);
    auto res = intr.find(nullptr);
    if (!res.m_hasIntersect) return ErrorCode::invalidChangingCamera;
    Vector3 new_loc = intr.GetPoint();
    error er = changeCameraFrame(new_loc, std::nullopt, std::nullopt);
    if (!er) EventPublisher::enqueue(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::cullingFrustum(const Frustum& frustum)
{
    m_cullingFrustum = frustum;

    return ErrorCode::ok;
}

void Camera::changeAspectRatio(float ratio)
{
    assert(m_cullingFrustum.projectionType() == Frustum::ProjectionType::Perspective);
    m_cullingFrustum = Frustum::fromPerspective(m_handSys, m_cullingFrustum.fov(), ratio, m_cullingFrustum.nearPlaneZ(), m_cullingFrustum.farPlaneZ());
}

void Camera::changeFrustumFarPlane(float far_z)
{
    if (m_cullingFrustum.projectionType() == Frustum::ProjectionType::Perspective)
    {
        m_cullingFrustum = Frustum::fromPerspective(m_handSys, m_cullingFrustum.fov(), m_cullingFrustum.aspectRatio(), m_cullingFrustum.nearPlaneZ(), far_z);
    }
    else
    {
        m_cullingFrustum = Frustum::fromOrtho(m_handSys, m_cullingFrustum.nearWidth(), m_cullingFrustum.nearHeight(), m_cullingFrustum.nearPlaneZ(), far_z);
    }
}

void Camera::changeFrustumNearPlane(float near_z)
{
    if (m_cullingFrustum.projectionType() == Frustum::ProjectionType::Perspective)
    {
        m_cullingFrustum = Frustum::fromPerspective(m_handSys, m_cullingFrustum.fov(), m_cullingFrustum.aspectRatio(), near_z, m_cullingFrustum.farPlaneZ());
    }
    else
    {
        m_cullingFrustum = Frustum::fromOrtho(m_handSys, m_cullingFrustum.nearWidth(), m_cullingFrustum.nearHeight(), near_z, m_cullingFrustum.farPlaneZ());
    }
}

void Camera::changeFrustumFov(Radian fov)
{
    assert(m_cullingFrustum.projectionType() == Frustum::ProjectionType::Perspective);
    m_cullingFrustum = Frustum::fromPerspective(m_handSys, fov, m_cullingFrustum.aspectRatio(), m_cullingFrustum.nearPlaneZ(), m_cullingFrustum.farPlaneZ());
}

void Camera::_updateViewTransform()
{
    Vector3 trans;
    trans.x() = m_vecRight.dot(m_vecLocation);
    trans.y() = m_vecUp.dot(m_vecLocation);
    trans.z() = m_vecCameraForward.dot(m_vecLocation);
    m_mxViewTransform = Matrix4(m_vecRight, m_vecUp, m_vecCameraForward, -trans, false);
}

const Matrix4& Camera::projectionTransform()
{
    return m_cullingFrustum.projectionTransform();
}

