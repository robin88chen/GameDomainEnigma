#include "GameCameraService.h"
#include "GameCameraEvents.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Frustum.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/IntrRay3Plane3.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, GameCameraService, ISystemService);

#define DEFAULT_CAMERA_POS Vector3(-5.0f, 5.0f, -5.0f)
#define WHEEL_THRESHOLD 120
#define PRIMARY_CAMERA_NAME "PrimaryCamera"
#define PRIMARY_FRUSTUM_NAME "PrimaryFrustum"

GameCameraService::GameCameraService(Frameworks::ServiceManager* mngr,
    const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = false;
    m_primaryCamera = nullptr;
    m_orderValue = ISystemService::ServiceDefaultOrderValue - 1;
}

GameCameraService::~GameCameraService()
{
    m_primaryCamera = nullptr;
}

ServiceResult GameCameraService::OnInit()
{
    CreatePrimaryCamera(DEFAULT_CAMERA_POS);
    return ServiceResult::Complete;
}

ServiceResult GameCameraService::OnTerm()
{
    m_primaryCamera = nullptr;
    return ServiceResult::Complete;
}

void GameCameraService::CreatePrimaryCamera(const MathLib::Vector3& vecCameraPos)
{
    assert(!m_sceneGraphRepository.expired());
    m_primaryCamera = m_sceneGraphRepository.lock()->CreateCamera(PRIMARY_CAMERA_NAME);
    m_primaryCamera->SetCullingFrustum(m_sceneGraphRepository.lock()->CreateFrustum(PRIMARY_FRUSTUM_NAME, Frustum::ProjectionType::Perspective));
    m_primaryCamera->SetCameraAxis(Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
    m_primaryCamera->SetCameraLocation(vecCameraPos);
    m_primaryCamera->GetCullingFrustum()->ChangeFarZ(300.0f);

    EventPublisher::Post(std::make_shared<GameCameraCreated>(m_primaryCamera));
}

void GameCameraService::CameraZoom(float dist)
{
    if (!m_primaryCamera) return;

    Vector3 dir = m_primaryCamera->GetEyeToLookatVector();
    Vector3 pos = m_primaryCamera->GetLocation();
    m_primaryCamera->SetCameraLocation(pos + dist * dir);

    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMove(float dir_dist, float slide_dist)
{
    if (!m_primaryCamera) return;
    Vector3 right = m_primaryCamera->GetRightVector();
    Vector3 up = Vector3::UNIT_Y;

    Vector3 move_dir = right.Cross(up);
    Vector3 move_right = up.Cross(move_dir);
    Vector3 pos = m_primaryCamera->GetLocation() + dir_dist * move_dir + slide_dist * move_right;

    m_primaryCamera->SetCameraLocation(pos);

    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMoveXZ(float move_x, float move_z)
{
    if (!m_primaryCamera) return;
    Vector3 pos = m_primaryCamera->GetLocation() + Vector3(move_x, 0.0f, move_z);

    m_primaryCamera->SetCameraLocation(pos);

    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraShiftLookAt(const MathLib::Vector3& vecLookAt)
{
    if (!m_primaryCamera) return;
    Vector3 dir = m_primaryCamera->GetEyeToLookatVector();
    Vector3 loc = m_primaryCamera->GetLocation();
    Plane3 horz_plane(Vector3::UNIT_Y, loc);
    Ray3 dir_ray(vecLookAt, -dir);
    IntrRay3Plane3 intr(dir_ray, horz_plane);
    bool res = intr.Find(0);
    if (!res) return;
    Vector3 new_loc = intr.GetPoint();
    m_primaryCamera->SetCameraLocation(new_loc);

    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraSphereRotate(float horz_angle, float vert_angle, const MathLib::Vector3& center)
{
    if (!m_primaryCamera) return;
    Vector3 vecCenter = center;
    if (center == Vector3::ZERO)
    {
        Vector3 eye = m_primaryCamera->GetLocation();
        Vector3 dir = m_primaryCamera->GetEyeToLookatVector();
        if (std::fabs(dir.Y()) > Math::Epsilon())
        {
            float t = (-eye.Y()) / dir.Y();
            vecCenter = eye + t * dir;
        }
    }
    // thx to george lu
    Vector3 right = m_primaryCamera->GetRightVector();
    right = Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * right;

    Vector3 dir = Quaternion(right, vert_angle) *
        Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * m_primaryCamera->GetEyeToLookatVector();    // 對XZ軸做旋轉
    Vector3 up = Quaternion(right, vert_angle) *
        Quaternion(Vector3(0.0f, 1.0f, 0.0f), horz_angle) * m_primaryCamera->GetUpVector(); // 對XZ軸做旋轉
    float radius;
    Vector3 dist = m_primaryCamera->GetLocation() - vecCenter;
    radius = dist.Length();
    Vector3 loc = vecCenter - radius * dir;                                         // 新的camera位置

    m_primaryCamera->SetCameraFrame(loc, dir, up);

    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::ChangeAspectRatio(float ratio)
{
    if (!m_primaryCamera) return;
    if (m_primaryCamera->GetCullingFrustum())
    {
        m_primaryCamera->GetCullingFrustum()->ChangeAspectRatio(ratio);
    }
    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::Aspect));
}

void GameCameraService::ChangeFrustumFarPlane(float far_z)
{
    if (!m_primaryCamera) return;
    FrustumPtr frustum = m_primaryCamera->GetCullingFrustum();
    if (!frustum) return;
    frustum->ChangeFarZ(far_z);
    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

void GameCameraService::ChangeFrustumFov(float fov)
{
    if (!m_primaryCamera) return;
    FrustumPtr frustum = m_primaryCamera->GetCullingFrustum();
    if (!frustum) return;
    frustum->ChangeFov(fov);
    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumFov));
}

void GameCameraService::ChangeFrustumNearPlane(float near_z)
{
    if (!m_primaryCamera) return;
    FrustumPtr frustum = m_primaryCamera->GetCullingFrustum();
    if (!frustum) return;
    frustum->ChangeNearZ(near_z);
    EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

Ray3 GameCameraService::GetPickerRay(float clip_space_x, float clip_space_y)
{
    assert(m_primaryCamera);
    assert(m_primaryCamera->GetCullingFrustum());
    Vector3 clip_vec = Vector3(clip_space_x, clip_space_y, 0.0f);
    Matrix4 mxProj = m_primaryCamera->GetCullingFrustum()->GetProjectionTransform().Inverse();
    Vector3 camera_vec = mxProj.TransformCoord(clip_vec);
    Matrix4 mxView = m_primaryCamera->GetViewTransform().Inverse();
    Vector3 world_vec = mxView.TransformCoord(camera_vec);

    Vector3 ray_dir = world_vec - m_primaryCamera->GetLocation();
    ray_dir.NormalizeSelf();

    return Ray3(m_primaryCamera->GetLocation(), ray_dir);
}
