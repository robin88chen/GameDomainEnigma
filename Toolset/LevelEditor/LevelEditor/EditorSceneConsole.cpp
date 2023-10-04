#include "EditorSceneConsole.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "InputHandlers/MouseInputEvents.h"
#include "InputHandlers/WinKeyboardInputEvents.h"
#include "Renderer/RendererEvents.h"
#include "GameCommon/GameSceneEvents.h"
#include "GameCommon/GameCameraEvents.h"
#include "GameCommon/GameCameraCommands.h"
#include "ScenePicker.h"
#include "LevelEditorEvents.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::InputHandlers;
using namespace Enigma::Renderer;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;

Rtti EditorSceneConsole::TYPE_RTTI{ "LevelEditor.EditorSceneConsole", &ISystemService::TYPE_RTTI };

EditorSceneConsole::EditorSceneConsole(Enigma::Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
}

EditorSceneConsole::~EditorSceneConsole()
{
}

Enigma::Frameworks::ServiceResult EditorSceneConsole::OnInit()
{
    m_onGameCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraCreated(e); });
    EventPublisher::Subscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    m_onSceneRootCreated = std::make_shared<EventSubscriber>([=](auto e) { OnSceneRootCreated(e); });
    EventPublisher::Subscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    m_onTargetViewportChanged = std::make_shared<EventSubscriber>([=](auto e) { OnTargetViewportChanged(e); });
    EventPublisher::Subscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onMouseMoved = std::make_shared<EventSubscriber>([=](auto e) { OnMouseMoved(e); });
    EventPublisher::Subscribe(typeid(MouseMoved), m_onMouseMoved);
    m_onMouseLeftButtonDown = std::make_shared<EventSubscriber>([=](auto e) { OnMouseLeftButtonDown(e); });
    EventPublisher::Subscribe(typeid(MouseLeftButtonDown), m_onMouseLeftButtonDown);
    m_onMouseLeftButtonUp = std::make_shared<EventSubscriber>([=](auto e) { OnMouseLeftButtonUp(e); });
    EventPublisher::Subscribe(typeid(MouseLeftButtonUp), m_onMouseLeftButtonUp);
    m_onMouseLeftDragged = std::make_shared<EventSubscriber>([=](auto e) { OnMouseLeftDragged(e); });
    EventPublisher::Subscribe(typeid(MouseLeftButtonDrag), m_onMouseLeftDragged);
    m_onKeyboardAsyncKeyPressed = std::make_shared<EventSubscriber>([=](auto e) { OnKeyboardAsyncKeyPressed(e); });
    EventPublisher::Subscribe(typeid(WinKeyboardAsyncPressed), m_onKeyboardAsyncKeyPressed);

    return ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult EditorSceneConsole::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    m_onGameCameraCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    m_onSceneRootCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onTargetViewportChanged = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseMoved), m_onMouseMoved);
    m_onMouseMoved = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseLeftButtonDown), m_onMouseLeftButtonDown);
    m_onMouseLeftButtonDown = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseLeftButtonUp), m_onMouseLeftButtonUp);
    m_onMouseLeftButtonUp = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseLeftButtonDrag), m_onMouseLeftDragged);
    m_onMouseLeftDragged = nullptr;
    EventPublisher::Unsubscribe(typeid(WinKeyboardAsyncPressed), m_onKeyboardAsyncKeyPressed);
    m_onKeyboardAsyncKeyPressed = nullptr;

    return ServiceResult::Complete;
}

void EditorSceneConsole::OnGameCameraCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::GameCommon::GameCameraCreated, IEvent>(e);
    if (!ev) return;
    m_camera = ev->GetCamera();
}

void EditorSceneConsole::OnSceneRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::GameCommon::SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
}
void EditorSceneConsole::OnTargetViewportChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::Renderer::TargetViewPortChanged, IEvent>(e);
    if (!ev) return;
    m_targetViewport = ev->GetViewPort();
}

void EditorSceneConsole::OnMouseMoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseMoved>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = PickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::Post(std::make_shared<SceneCursorMoved>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::OnMouseLeftButtonDown(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDown>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = PickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::Post(std::make_shared<SceneCursorPressed>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::OnMouseLeftButtonUp(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonUp>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = PickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::Post(std::make_shared<SceneCursorReleased>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::OnMouseLeftDragged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDrag>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = PickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::Post(std::make_shared<SceneCursorDragged>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::OnKeyboardAsyncKeyPressed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WinKeyboardAsyncPressed>(e);
    if (!ev) return;
    if (ev->m_param.m_virtualKey == 'A')
    {
        CommandBus::Post(std::make_shared<MoveCamera>(0.0f, -0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'D')
    {
        CommandBus::Post(std::make_shared<MoveCamera>(0.0f, 0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'S')
    {
        CommandBus::Post(std::make_shared<MoveCamera>(-0.1f, 0.0f));
    }
    else if (ev->m_param.m_virtualKey == 'W')
    {
        CommandBus::Post(std::make_shared<MoveCamera>(0.1f, 0.0f));
    }
}

std::tuple<std::shared_ptr<Enigma::SceneGraph::Pawn>, Enigma::MathLib::Vector3> EditorSceneConsole::PickingOnSceneView(const Enigma::MathLib::Vector2& clip_pos)
{
    if (m_camera.expired()) return { nullptr, Enigma::MathLib::Vector3::ZERO };
    if (m_sceneRoot.expired()) return { nullptr, Vector3::ZERO };

    m_scenePicker.SetAssociatedCamera(m_camera.lock());
    m_scenePicker.SetClippingCoord(clip_pos);
    m_scenePicker.Picking(m_sceneRoot.lock());

    const unsigned int count = m_scenePicker.GetRecordCount();
    if (count == 0)
    {
        return { nullptr, Vector3::ZERO };
    }

    std::shared_ptr<Enigma::SceneGraph::Pawn> pickedPawn = nullptr;
    Vector3 picked_pos;
    for (unsigned int i = 0; i < count; i++)
    {
        auto pick_record = m_scenePicker.GetPickRecord(i);
        if (!pick_record) continue;
        if (pick_record->m_spatial.expired()) continue;
        pickedPawn = std::dynamic_pointer_cast<Enigma::SceneGraph::Pawn, Enigma::SceneGraph::Spatial>(pick_record->m_spatial.lock());
        picked_pos = pick_record->m_vecIntersection;
        if (pickedPawn) break;
    }
    return { pickedPawn, picked_pos };
}