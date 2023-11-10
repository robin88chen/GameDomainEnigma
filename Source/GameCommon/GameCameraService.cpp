#include "Platforms/PlatformConfig.h"
#include "GameCameraService.h"
#include "GameCameraEvents.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Frustum.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "MathLib/IntrRay3Plane3.h"
#include "Renderer/RendererEvents.h"
#include "Renderer/RenderTarget.h"
#include "InputHandlers/MouseInputEvents.h"
#include "GameCameraCommands.h"
#include "InputHandlers/GestureInputEvents.h"

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
}

GameCameraService::~GameCameraService()
{
    m_primaryCamera = nullptr;
}

ServiceResult GameCameraService::onInit()
{
    m_onTargetResized = std::make_shared<EventSubscriber>([=](auto e) { OnTargetResized(e); });
    EventPublisher::subscribe(typeid(RenderTargetResized), m_onTargetResized);

    m_doZoomingCamera = std::make_shared<CommandSubscriber>([=](auto c) { DoZoomingCamera(c); });
    CommandBus::subscribe(typeid(ZoomCamera), m_doZoomingCamera);
    m_doSphereRotatingCamera = std::make_shared<CommandSubscriber>([=](auto c) { DoSphereRotatingCamera(c); });
    CommandBus::subscribe(typeid(SphereRotateCamera), m_doSphereRotatingCamera);
    m_doMovingCamera = std::make_shared<CommandSubscriber>([=](auto c) { DoMovingCamera(c); });
    CommandBus::subscribe(typeid(MoveCamera), m_doMovingCamera);
    m_doMovingCameraXZ = std::make_shared<CommandSubscriber>([=](auto c) { DoMovingCameraXZ(c); });
    CommandBus::subscribe(typeid(MoveCameraXZ), m_doMovingCameraXZ);

#if TARGET_PLATFORM == PLATFORM_WIN32
    m_onRightBtnDrag = std::make_shared<EventSubscriber>([=](auto e) { OnMouseRightBtnDrag(e); });
    EventPublisher::subscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    m_onMouseWheel = std::make_shared<EventSubscriber>([=](auto e) { OnMouseWheel(e); });
    EventPublisher::subscribe(typeid(MouseWheeled), m_onMouseWheel);
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
    m_onGestureScroll = std::make_shared<EventSubscriber>([=](auto e) { OnGestureScroll(e); });
    EventPublisher::subscribe(typeid(GestureScroll), m_onGestureScroll);
    m_onGestureScale = std::make_shared<EventSubscriber>([=](auto e) { OnGestureScale(e); });
    EventPublisher::subscribe(typeid(GestureScale), m_onGestureScale);
#endif
    return ServiceResult::Complete;
}

ServiceResult GameCameraService::onTerm()
{
    EventPublisher::unsubscribe(typeid(RenderTargetResized), m_onTargetResized);
    m_onTargetResized = nullptr;

    CommandBus::unsubscribe(typeid(ZoomCamera), m_doZoomingCamera);
    m_doZoomingCamera = nullptr;
    CommandBus::unsubscribe(typeid(SphereRotateCamera), m_doSphereRotatingCamera);
    m_doSphereRotatingCamera = nullptr;
    CommandBus::unsubscribe(typeid(MoveCamera), m_doMovingCamera);
    m_doMovingCamera = nullptr;
    CommandBus::unsubscribe(typeid(MoveCameraXZ), m_doMovingCameraXZ);
    m_doMovingCameraXZ = nullptr;

#if TARGET_PLATFORM == PLATFORM_WIN32
    EventPublisher::unsubscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    m_onRightBtnDrag = nullptr;
    EventPublisher::unsubscribe(typeid(MouseWheeled), m_onMouseWheel);
    m_onMouseWheel = nullptr;
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
    EventPublisher::unsubscribe(typeid(GestureScroll), m_onGestureScroll);
    m_onGestureScroll = nullptr;
    EventPublisher::unsubscribe(typeid(GestureScale), m_onGestureScale);
    m_onGestureScale = nullptr;
#endif

    m_primaryCamera = nullptr;

    return ServiceResult::Complete;
}

void GameCameraService::CreatePrimaryCamera(const Engine::GenericDto& dto)
{
    assert(!m_sceneGraphRepository.expired());
    m_primaryCamera = m_sceneGraphRepository.lock()->CreateCamera(dto);
    EventPublisher::post(std::make_shared<GameCameraCreated>(m_primaryCamera));
}

void GameCameraService::CameraZoom(float dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->zoom(dist);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMove(float dir_dist, float slide_dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->move(dir_dist, slide_dist);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraMoveXZ(float move_x, float move_z)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->moveXZ(move_x, move_z);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraShiftLookAt(const Vector3& vecLookAt)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->shiftLookAt(vecLookAt);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::CameraSphereRotate(float horz_angle, float vert_angle, const Vector3& center)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->sphereRotate(horz_angle, vert_angle, center);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::ChangeAspectRatio(float ratio)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeAspectRatio(ratio);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::Aspect));
}

void GameCameraService::ChangeFrustumFarPlane(float far_z)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumFarPlane(far_z);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

void GameCameraService::ChangeFrustumFov(float fov)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumFov(fov);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumFov));
}

void GameCameraService::ChangeFrustumNearPlane(float near_z)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumNearPlane(near_z);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

Ray3 GameCameraService::GetPickerRay(float clip_space_x, float clip_space_y)
{
    assert(m_primaryCamera);
    Vector3 clip_vec = Vector3(clip_space_x, clip_space_y, 0.0f);
    Matrix4 mxProj = m_primaryCamera->cullingFrustum().projectionTransform().Inverse();
    Vector3 camera_vec = mxProj.TransformCoord(clip_vec);
    Matrix4 mxView = m_primaryCamera->viewTransform().Inverse();
    Vector3 world_vec = mxView.TransformCoord(camera_vec);

    Vector3 ray_dir = world_vec - m_primaryCamera->location();
    ray_dir.NormalizeSelf();

    return Ray3(m_primaryCamera->location(), ray_dir);
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

void GameCameraService::DoZoomingCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ZoomCamera, ICommand>(c);
    if (!cmd) return;
    CameraZoom(cmd->GetDistance());
}

void GameCameraService::DoSphereRotatingCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<SphereRotateCamera, ICommand>(c);
    if (!cmd) return;
    CameraSphereRotate(cmd->GetHorizontalAngle(), cmd->GetVerticalAngle(), cmd->GetCenter());
}

void GameCameraService::DoMovingCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<MoveCamera, ICommand>(c);
    if (!cmd) return;
    CameraMove(cmd->GetDirectionDistance(), cmd->GetSlideDistance());
}

void GameCameraService::DoMovingCameraXZ(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<MoveCameraXZ, ICommand>(c);
    if (!cmd) return;
    CameraMoveXZ(cmd->GetMoveX(), cmd->GetMoveZ());
}
