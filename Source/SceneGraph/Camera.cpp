#include "Camera.h"
#include "SceneGraphErrors.h"
#include "CameraFrustumDtos.h"
#include "Frustum.h"
#include "CameraFrustumEvents.h"
#include "MathLib/Ray3.h"
#include "MathLib/IntrRay3Plane3.h"
#include "MathLib/MathGlobal.h"
#include "Frameworks/EventPublisher.h"
#include <cassert>
#include <memory>

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(SceneGraph, Camera);

Camera::Camera(const std::string& name, GraphicCoordSys hand) : m_factoryDesc(Camera::TYPE_RTTI.GetName())
{
    m_name = name;
    m_handSys = hand;
    m_cullingFrustum = nullptr;
    m_vecLocation = Vector3::ZERO;
    m_vecEyeToLookAt = Vector3::UNIT_Z;
    m_vecUp = Vector3::UNIT_Y;
    m_vecRight = Vector3::UNIT_X;
}

Camera::Camera(const CameraDto& dto) : m_factoryDesc(dto.TheFactoryDesc())
{
    m_name = dto.Name();
    m_handSys = dto.HandSystem();
    ChangeCameraFrame(dto.EyePosition(), dto.LookAtDirection(), dto.UpVector());
    //todo : 要改成 lazy load
    m_cullingFrustum = std::make_shared<Frustum>(FrustumDto::FromGenericDto(dto.Frustum()));
}

Camera::~Camera()
{
    m_cullingFrustum = nullptr;
}

GenericDto Camera::SerializeDto()
{
    CameraDto dto;
    dto.TheFactoryDesc() = m_factoryDesc;
    dto.Name() = m_name;
    dto.HandSystem() = m_handSys;
    dto.EyePosition() = m_vecLocation;
    dto.LookAtDirection() = m_vecEyeToLookAt;
    dto.UpVector() = m_vecUp;
    dto.Frustum() = m_cullingFrustum->SerializeDto();
    return dto.ToGenericDto();
}

error Camera::ChangeCameraFrame(const std::optional<Vector3>& eye,
    const std::optional<Vector3>& eye_to_lookat, const std::optional<Vector3>& up)
{
    if (eye)
    {
        m_vecLocation = eye.value();
    }
    assert(eye_to_lookat.has_value() == up.has_value());  // must be both set or both not set
    if (eye_to_lookat)
    {
        Vector3 _eye_to_lookat = eye_to_lookat.value().Normalize();
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
        Vector3 _up = up.value().Normalize();
        Vector3 _right = _up.Cross(m_vecCameraForward);
        m_vecRight = _right.Normalize();
        m_vecUp = m_vecCameraForward.Cross(m_vecRight);
    }

    _UpdateViewTransform();
    return ErrorCode::ok;
}

error Camera::Zoom(float dist)
{
    error er = ChangeCameraFrame(m_vecLocation + dist * m_vecEyeToLookAt, std::nullopt, std::nullopt);
    if (!er) EventPublisher::Post(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::SphereRotate(float horz_angle, float vert_angle, const Vector3& center)
{
    Vector3 vecCenter = center;
    if (center == Vector3::ZERO)
    {
        Vector3 eye = m_vecLocation;
        Vector3 dir = m_vecEyeToLookAt;
        if (std::fabs(dir.Y()) > Math::Epsilon())
        {
            float t = (-eye.Y()) / dir.Y();
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
    radius = dist.Length();
    Vector3 loc = vecCenter - radius * dir;  // 新的camera位置

    error er = ChangeCameraFrame(loc, dir, up);
    if (!er) EventPublisher::Post(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::Move(float dir_dist, float slide_dist)
{
    Vector3 right = m_vecRight;
    Vector3 up = Vector3::UNIT_Y;

    Vector3 move_dir = right.Cross(up);
    Vector3 move_right = up.Cross(move_dir);
    Vector3 pos = m_vecLocation + dir_dist * move_dir + slide_dist * move_right;

    error er = ChangeCameraFrame(pos, std::nullopt, std::nullopt);
    if (!er) EventPublisher::Post(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::MoveXZ(float move_x, float move_z)
{
    error er = ChangeCameraFrame(m_vecLocation + Vector3(move_x, 0.0f, move_z), std::nullopt, std::nullopt);
    if (!er) EventPublisher::Post(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::ShiftLookAt(const Vector3& vecLookAt)
{
    Vector3 dir = m_vecEyeToLookAt;
    Vector3 loc = m_vecLocation;
    Plane3 horz_plane(Vector3::UNIT_Y, loc);
    Ray3 dir_ray(vecLookAt, -dir);
    IntrRay3Plane3 intr(dir_ray, horz_plane);
    bool res = intr.Find(0);
    if (!res) return ErrorCode::invalidChangingCamera;
    Vector3 new_loc = intr.GetPoint();
    error er = ChangeCameraFrame(new_loc, std::nullopt, std::nullopt);
    if (!er) EventPublisher::Post(std::make_shared<CameraFrameChanged>(shared_from_this()));
    return er;
}

error Camera::SetCullingFrustum(const FrustumPtr& frustum)
{
    assert(frustum);  // must valid pointer
    m_cullingFrustum = frustum;

    return ErrorCode::ok;
}

void Camera::_UpdateViewTransform()
{
    Vector3 trans;
    trans.X() = m_vecRight.Dot(m_vecLocation);
    trans.Y() = m_vecUp.Dot(m_vecLocation);
    trans.Z() = m_vecCameraForward.Dot(m_vecLocation);
    m_mxViewTransform = Matrix4(m_vecRight, m_vecUp, m_vecCameraForward, -trans, false);
}

const Matrix4& Camera::GetProjectionTransform()
{
    return m_cullingFrustum->GetProjectionTransform();
}
