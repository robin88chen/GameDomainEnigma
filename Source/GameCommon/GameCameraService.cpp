#include "Platforms/PlatformConfig.h"
#include "GameCameraService.h"
#include "GameCameraEvents.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Frustum.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/IntrRay3Plane3.h"
#include "Renderer/RendererEvents.h"
#include "Renderer/RenderTarget.h"
#include "InputHandlers/MouseInputEvents.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::InputHandlers;

DEFINE_RTTI(GameCommon, GameCameraService, ISystemService);

#define DEFAULT_CAMERA_POS Vector3(-5.0f, 5.0f, -5.0f)
#define WHEEL_THRESHOLD 120
#define PRIMARY_CAMERA_NAME "PrimaryCamera"
#define PRIMARY_FRUSTUM_NAME "PrimaryFrustum"

GameCameraService::GameCameraService(ServiceManager* mngr,
                                     const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = false;
    m_primaryCamera = nullptr;
    m_orderValue = ServiceDefaultOrderValue - 1;
}

GameCameraService::~GameCameraService()
{
    m_primaryCamera = nullptr;
}

ServiceResult GameCameraService::OnInit()
{
    CreatePrimaryCamera(DEFAULT_CAMERA_POS);

    m_onTargetResized = std::make_shared<EventSubscriber>([=](auto e) { OnTargetResized(e); });
    m_onRightBtnDrag = std::make_shared<EventSubscriber>([=](auto e) { OnMouseRightBtnDrag(e); });
    m_onMouseWheel = std::make_shared<EventSubscriber>([=](auto e) { OnMouseWheel(e); });
    EventPublisher::Subscribe(typeid(RenderTargetResized), m_onTargetResized);
#if TARGET_PLATFORM == PLATFORM_WIN32
    EventPublisher::Subscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    EventPublisher::Subscribe(typeid(MouseWheeled), m_onMouseWheel);
#endif
    return ServiceResult::Complete;
}

ServiceResult GameCameraService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(RenderTargetResized), m_onTargetResized);
#if TARGET_PLATFORM == PLATFORM_WIN32
    EventPublisher::Unsubscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    EventPublisher::Unsubscribe(typeid(MouseWheeled), m_onMouseWheel);
#endif
    m_onTargetResized = nullptr;
    m_onRightBtnDrag = nullptr;
    m_onMouseWheel = nullptr;

    m_primaryCamera = nullptr;

    return ServiceResult::Complete;
}

void GameCameraService::CreatePrimaryCamera(const Vector3& vecCameraPos)
{
    assert(!m_sceneGraphRepository.expired());
    m_primaryCamera = m_sceneGraphRepository.lock()->CreateCamera(PRIMARY_CAMERA_NAME);
    m_primaryCamera->SetCullingFrustum(m_sceneGraphRepository.lock()->CreateFrustum(PRIMARY_FRUSTUM_NAME, Frustum::ProjectionType::Perspective));
    m_primaryCamera->ChangeCameraFrame(vecCameraPos, Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
    m_primaryCamera->GetCullingFrustum()->ChangeFarZ(300.0f);

    EventPublisher::Post(std::make_shared<GameCameraCreated>(m_primaryCamera));
}

void GameCameraService::CameraZoom(float dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->Zoom(dist);

    if (!er) EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMove(float dir_dist, float slide_dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->Move(dir_dist, slide_dist);

    if (!er) EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMoveXZ(float move_x, float move_z)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->MoveXZ(move_x, move_z);

    if (!er) EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraShiftLookAt(const Vector3& vecLookAt)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->ShiftLookAt(vecLookAt);

    if (!er) EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraSphereRotate(float horz_angle, float vert_angle, const Vector3& center)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->SphereRotate(horz_angle, vert_angle, center);

    if (!er) EventPublisher::Post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
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

void GameCameraService::OnTargetResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRenderTarget() == nullptr) return;
    if (!ev->GetRenderTarget()->IsPrimary()) return;
    ChangeAspectRatio((float)ev->GetDimension().m_width / (float)ev->GetDimension().m_height);
}

void GameCameraService::OnMouseRightBtnDrag(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseRightButtonDrag, IEvent>(e);
    if (!ev) return;
    CameraSphereRotate(ev->m_param.m_deltaX * 0.003f, ev->m_param.m_deltaY * 0.003f);
#endif
}

void GameCameraService::OnMouseWheel(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseWheeled, IEvent>(e);
    if (!ev) return;
    float delta_dist = (float)(ev->m_param.m_deltaWheel / WHEEL_THRESHOLD) * 0.5f;
    CameraZoom(delta_dist);
#endif
}
