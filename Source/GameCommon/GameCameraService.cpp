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
#include "InputHandlers/GestureInputEvents.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "GameCommonErrors.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::InputHandlers;

DEFINE_RTTI(GameCommon, GameCameraService, ISystemService);

#define WHEEL_THRESHOLD 120.0f

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
    m_onFrustumCreated = std::make_shared<EventSubscriber>([=](auto e) { OnCameraFrustumCreated(e); });
    EventPublisher::Subscribe(typeid(FrustumCreated), m_onFrustumCreated);
    m_onCreateFrustumFailed = std::make_shared<EventSubscriber>([=](auto e) { OnCreateCameraFrustumFailed(e); });
    EventPublisher::Subscribe(typeid(CreateFrustumFailed), m_onCreateFrustumFailed);
    m_onTargetResized = std::make_shared<EventSubscriber>([=](auto e) { OnTargetResized(e); });
    EventPublisher::Subscribe(typeid(RenderTargetResized), m_onTargetResized);
#if TARGET_PLATFORM == PLATFORM_WIN32
    m_onRightBtnDrag = std::make_shared<EventSubscriber>([=](auto e) { OnMouseRightBtnDrag(e); });
    EventPublisher::Subscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    m_onMouseWheel = std::make_shared<EventSubscriber>([=](auto e) { OnMouseWheel(e); });
    EventPublisher::Subscribe(typeid(MouseWheeled), m_onMouseWheel);
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
    m_onGestureScroll = std::make_shared<EventSubscriber>([=](auto e) { OnGestureScroll(e); });
    EventPublisher::Subscribe(typeid(GestureScroll), m_onGestureScroll);
    m_onGestureScale = std::make_shared<EventSubscriber>([=](auto e) { OnGestureScale(e); });
    EventPublisher::Subscribe(typeid(GestureScale), m_onGestureScale);
#endif
    return ServiceResult::Complete;
}

ServiceResult GameCameraService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(FrustumCreated), m_onFrustumCreated);
    m_onFrustumCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(CreateFrustumFailed), m_onCreateFrustumFailed);
    m_onCreateFrustumFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(RenderTargetResized), m_onTargetResized);
    m_onTargetResized = nullptr;
#if TARGET_PLATFORM == PLATFORM_WIN32
    EventPublisher::Unsubscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    m_onRightBtnDrag = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseWheeled), m_onMouseWheel);
    m_onMouseWheel = nullptr;
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
    EventPublisher::Unsubscribe(typeid(GestureScroll), m_onGestureScroll);
    m_onGestureScroll = nullptr;
    EventPublisher::Unsubscribe(typeid(GestureScale), m_onGestureScale);
    m_onGestureScale = nullptr;
#endif

    m_primaryCamera = nullptr;

    return ServiceResult::Complete;
}

void GameCameraService::CreatePrimaryCamera(const Engine::GenericDto& dto)
{
    assert(!m_sceneGraphRepository.expired());
    m_primaryCamera = m_sceneGraphRepository.lock()->CreateCamera(dto);
    if (m_primaryCamera->GetCullingFrustum())
    {
        EventPublisher::Post(std::make_shared<GameCameraCreated>(m_primaryCamera));
    }
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

void GameCameraService::OnCameraFrustumCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::FrustumCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_primaryCamera->GetCullingFrustumName()) return;
    if (ev->GetFrustum())
    {
        EventPublisher::Post(std::make_shared<GameCameraCreated>(m_primaryCamera));
    }
    else
    {
        EventPublisher::Post(std::make_shared<CreateGameCameraFailed>(ev->GetName(), ErrorCode::nullFrustum));
    }
}

void GameCameraService::OnCreateCameraFrustumFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::CreateFrustumFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_primaryCamera->GetCullingFrustumName()) return;
    EventPublisher::Post(std::make_shared<CreateGameCameraFailed>(ev->GetName(), ev->GetError()));
}

void GameCameraService::OnTargetResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRenderTarget() == nullptr) return;
    if (!ev->GetRenderTarget()->IsPrimary()) return;
    ChangeAspectRatio(static_cast<float>(ev->GetDimension().m_width) / static_cast<float>(ev->GetDimension().m_height));
}

void GameCameraService::OnMouseRightBtnDrag(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseRightButtonDrag, IEvent>(e);
    if (!ev) return;
    CameraSphereRotate(static_cast<float>(ev->m_param.m_deltaX) * 0.003f, static_cast<float>(ev->m_param.m_deltaY) * 0.003f);
#endif
}

void GameCameraService::OnMouseWheel(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseWheeled, IEvent>(e);
    if (!ev) return;
    float delta_dist = (static_cast<float>(ev->m_param.m_deltaWheel) / WHEEL_THRESHOLD) * 0.5f;
    CameraZoom(delta_dist);
#endif
}

void GameCameraService::OnGestureScroll(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GestureScroll, IEvent>(e);
    if (!ev) return;
    CameraSphereRotate(ev->m_delta.X() * 0.001f, -ev->m_delta.Y() * 0.001f);
#endif
}

void GameCameraService::OnGestureScale(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GestureScale, IEvent>(e);
    if (!ev) return;
    float delta_dist = (ev->m_factor - 1.0f) * 10.0f;
    CameraZoom(delta_dist);
#endif
}
