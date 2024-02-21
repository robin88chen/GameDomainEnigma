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
#include "SceneGraph/SceneGraphFactory.h"

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
    m_onTargetResized = std::make_shared<EventSubscriber>([=](auto e) { onTargetResized(e); });
    EventPublisher::subscribe(typeid(RenderTargetResized), m_onTargetResized);

    m_zoomCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { zoom(c); });
    CommandBus::subscribe(typeid(ZoomCamera), m_zoomCamera);
    m_sphereRotateCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { sphereRotate(c); });
    CommandBus::subscribe(typeid(SphereRotateCamera), m_sphereRotateCamera);
    m_moveCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { move(c); });
    CommandBus::subscribe(typeid(MoveCamera), m_moveCamera);
    m_moveCameraXZ = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { moveXZ(c); });
    CommandBus::subscribe(typeid(MoveCameraXZ), m_moveCameraXZ);

#if TARGET_PLATFORM == PLATFORM_WIN32
    m_onRightBtnDrag = std::make_shared<EventSubscriber>([=](auto e) { onMouseRightBtnDrag(e); });
    EventPublisher::subscribe(typeid(MouseRightButtonDrag), m_onRightBtnDrag);
    m_onMouseWheel = std::make_shared<EventSubscriber>([=](auto e) { onMouseWheel(e); });
    EventPublisher::subscribe(typeid(MouseWheeled), m_onMouseWheel);
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
    m_onGestureScroll = std::make_shared<EventSubscriber>([=](auto e) { onGestureScroll(e); });
    EventPublisher::subscribe(typeid(GestureScroll), m_onGestureScroll);
    m_onGestureScale = std::make_shared<EventSubscriber>([=](auto e) { onGestureScale(e); });
    EventPublisher::subscribe(typeid(GestureScale), m_onGestureScale);
#endif

    return ServiceResult::Complete;
}

ServiceResult GameCameraService::onTerm()
{
    EventPublisher::unsubscribe(typeid(RenderTargetResized), m_onTargetResized);
    m_onTargetResized = nullptr;

    CommandBus::unsubscribe(typeid(ZoomCamera), m_zoomCamera);
    m_zoomCamera = nullptr;
    CommandBus::unsubscribe(typeid(SphereRotateCamera), m_sphereRotateCamera);
    m_sphereRotateCamera = nullptr;
    CommandBus::unsubscribe(typeid(MoveCamera), m_moveCamera);
    m_moveCamera = nullptr;
    CommandBus::unsubscribe(typeid(MoveCameraXZ), m_moveCameraXZ);
    m_moveCameraXZ = nullptr;

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

void GameCameraService::constitutePrimaryCamera(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    assert(!m_sceneGraphRepository.expired());
    if (m_sceneGraphRepository.lock()->hasCamera(id))
    {
        m_primaryCamera = m_sceneGraphRepository.lock()->queryCamera(id);
    }
    else
    {
        m_primaryCamera = m_sceneGraphRepository.lock()->factory()->constituteCamera(id, dto, false);
        m_sceneGraphRepository.lock()->putCamera(m_primaryCamera);
    }
    assert(m_sceneGraphRepository.lock()->hasCamera(id));
    EventPublisher::post(std::make_shared<GameCameraCreated>(m_primaryCamera));
}

void GameCameraService::zoom(float dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->zoom(dist);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::move(float dir_dist, float slide_dist)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->move(dir_dist, slide_dist);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::moveXZ(float move_x, float move_z)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->moveXZ(move_x, move_z);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::shiftLookAt(const Vector3& vecLookAt)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->shiftLookAt(vecLookAt);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::sphereRotate(float horz_angle, float vert_angle, const Vector3& center)
{
    if (!m_primaryCamera) return;

    const error er = m_primaryCamera->sphereRotate(horz_angle, vert_angle, center);

    if (!er) EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::CameraFrame));
}

void GameCameraService::changeAspectRatio(float ratio)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeAspectRatio(ratio);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::Aspect));
}

void GameCameraService::changeFrustumFarPlane(float far_z)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumFarPlane(far_z);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

void GameCameraService::changeFrustumFov(float fov)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumFov(fov);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumFov));
}

void GameCameraService::changeFrustumNearPlane(float near_z)
{
    if (!m_primaryCamera) return;
    m_primaryCamera->changeFrustumNearPlane(near_z);
    EventPublisher::post(std::make_shared<GameCameraUpdated>(m_primaryCamera, GameCameraUpdated::NotifyCode::FrustumZ));
}

Ray3 GameCameraService::getPickerRay(float clip_space_x, float clip_space_y)
{
    assert(m_primaryCamera);
    Vector3 clip_vec = Vector3(clip_space_x, clip_space_y, 0.0f);
    Matrix4 mxProj = m_primaryCamera->cullingFrustum().projectionTransform().Inverse();
    Vector3 camera_vec = mxProj.TransformCoord(clip_vec);
    Matrix4 mxView = m_primaryCamera->viewTransform().Inverse();
    Vector3 world_vec = mxView.TransformCoord(camera_vec);

    Vector3 ray_dir = world_vec - m_primaryCamera->location();
    ray_dir.normalizeSelf();

    return Ray3(m_primaryCamera->location(), ray_dir);
}

void GameCameraService::onTargetResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    if (ev->getRenderTarget() == nullptr) return;
    if (!ev->getRenderTarget()->isPrimary()) return;
    changeAspectRatio(static_cast<float>(ev->getDimension().m_width) / static_cast<float>(ev->getDimension().m_height));
}

void GameCameraService::onMouseRightBtnDrag(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseRightButtonDrag, IEvent>(e);
    if (!ev) return;
    sphereRotate(static_cast<float>(ev->m_param.m_deltaX) * 0.003f, static_cast<float>(ev->m_param.m_deltaY) * 0.003f);
#endif
}

void GameCameraService::onMouseWheel(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseWheeled, IEvent>(e);
    if (!ev) return;
    float delta_dist = (static_cast<float>(ev->m_param.m_deltaWheel) / WHEEL_THRESHOLD) * 0.5f;
    zoom(delta_dist);
#endif
}

void GameCameraService::onGestureScroll(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GestureScroll, IEvent>(e);
    if (!ev) return;
    sphereRotate(ev->m_delta.x() * 0.001f, -ev->m_delta.y() * 0.001f);
#endif
}

void GameCameraService::onGestureScale(const Frameworks::IEventPtr& e)
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GestureScale, IEvent>(e);
    if (!ev) return;
    float delta_dist = (ev->m_factor - 1.0f) * 10.0f;
    zoom(delta_dist);
#endif
}

void GameCameraService::zoom(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ZoomCamera, ICommand>(c);
    if (!cmd) return;
    zoom(cmd->GetDistance());
}

void GameCameraService::sphereRotate(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<SphereRotateCamera, ICommand>(c);
    if (!cmd) return;
    sphereRotate(cmd->GetHorizontalAngle(), cmd->GetVerticalAngle(), cmd->GetCenter());
}

void GameCameraService::move(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<MoveCamera, ICommand>(c);
    if (!cmd) return;
    move(cmd->GetDirectionDistance(), cmd->GetSlideDistance());
}

void GameCameraService::moveXZ(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<MoveCameraXZ, ICommand>(c);
    if (!cmd) return;
    moveXZ(cmd->GetMoveX(), cmd->GetMoveZ());
}
