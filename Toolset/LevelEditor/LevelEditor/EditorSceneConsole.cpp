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

Enigma::Frameworks::ServiceResult EditorSceneConsole::onInit()
{
    m_onGameCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { onGameCameraCreated(e); });
    EventPublisher::subscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    //m_onSceneRootCreated = std::make_shared<EventSubscriber>([=](auto e) { onSceneRootCreated(e); });
    //EventPublisher::subscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    m_onTargetViewportChanged = std::make_shared<EventSubscriber>([=](auto e) { onTargetViewportChanged(e); });
    EventPublisher::subscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onMouseMoved = std::make_shared<EventSubscriber>([=](auto e) { onMouseMoved(e); });
    EventPublisher::subscribe(typeid(MouseMoved), m_onMouseMoved);
    m_onMouseLeftButtonDown = std::make_shared<EventSubscriber>([=](auto e) { onMouseLeftButtonDown(e); });
    EventPublisher::subscribe(typeid(MouseLeftButtonDown), m_onMouseLeftButtonDown);
    m_onMouseLeftButtonUp = std::make_shared<EventSubscriber>([=](auto e) { onMouseLeftButtonUp(e); });
    EventPublisher::subscribe(typeid(MouseLeftButtonUp), m_onMouseLeftButtonUp);
    m_onMouseLeftDragged = std::make_shared<EventSubscriber>([=](auto e) { onMouseLeftDragged(e); });
    EventPublisher::subscribe(typeid(MouseLeftButtonDrag), m_onMouseLeftDragged);
    m_onKeyboardAsyncKeyPressed = std::make_shared<EventSubscriber>([=](auto e) { onKeyboardAsyncKeyPressed(e); });
    EventPublisher::subscribe(typeid(WinKeyboardAsyncPressed), m_onKeyboardAsyncKeyPressed);

    return ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult EditorSceneConsole::onTerm()
{
    EventPublisher::unsubscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    m_onGameCameraCreated = nullptr;
    //EventPublisher::unsubscribe(typeid(SceneRootCreated), m_onSceneRootCreated);
    //m_onSceneRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onTargetViewportChanged = nullptr;
    EventPublisher::unsubscribe(typeid(MouseMoved), m_onMouseMoved);
    m_onMouseMoved = nullptr;
    EventPublisher::unsubscribe(typeid(MouseLeftButtonDown), m_onMouseLeftButtonDown);
    m_onMouseLeftButtonDown = nullptr;
    EventPublisher::unsubscribe(typeid(MouseLeftButtonUp), m_onMouseLeftButtonUp);
    m_onMouseLeftButtonUp = nullptr;
    EventPublisher::unsubscribe(typeid(MouseLeftButtonDrag), m_onMouseLeftDragged);
    m_onMouseLeftDragged = nullptr;
    EventPublisher::unsubscribe(typeid(WinKeyboardAsyncPressed), m_onKeyboardAsyncKeyPressed);
    m_onKeyboardAsyncKeyPressed = nullptr;

    return ServiceResult::Complete;
}

void EditorSceneConsole::onGameCameraCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::GameCommon::GameCameraCreated, IEvent>(e);
    if (!ev) return;
    m_camera = ev->GetCamera();
}

/*void EditorSceneConsole::onSceneRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::GameCommon::SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
}*/
void EditorSceneConsole::onTargetViewportChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::Renderer::TargetViewPortChanged, IEvent>(e);
    if (!ev) return;
    m_targetViewport = ev->viewPort();
}

void EditorSceneConsole::onMouseMoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseMoved>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::post(std::make_shared<SceneCursorMoved>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftButtonDown(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDown>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::post(std::make_shared<SceneCursorPressed>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftButtonUp(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonUp>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::post(std::make_shared<SceneCursorReleased>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftDragged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDrag>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Enigma::MathLib::Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::post(std::make_shared<SceneCursorDragged>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onKeyboardAsyncKeyPressed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WinKeyboardAsyncPressed>(e);
    if (!ev) return;
    if (ev->m_param.m_virtualKey == 'A')
    {
        CommandBus::post(std::make_shared<MoveCamera>(0.0f, -0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'D')
    {
        CommandBus::post(std::make_shared<MoveCamera>(0.0f, 0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'S')
    {
        CommandBus::post(std::make_shared<MoveCamera>(-0.1f, 0.0f));
    }
    else if (ev->m_param.m_virtualKey == 'W')
    {
        CommandBus::post(std::make_shared<MoveCamera>(0.1f, 0.0f));
    }
}

std::tuple<std::shared_ptr<Enigma::SceneGraph::Pawn>, Enigma::MathLib::Vector3> EditorSceneConsole::pickingOnSceneView(const Enigma::MathLib::Vector2& clip_pos)
{
    if (m_camera.expired()) return { nullptr, Enigma::MathLib::Vector3::ZERO };
    if (m_sceneRoot.expired()) return { nullptr, Vector3::ZERO };

    m_scenePicker.setAssociatedCamera(m_camera.lock());
    m_scenePicker.setClippingCoord(clip_pos);
    m_scenePicker.picking(m_sceneRoot.lock());

    const unsigned int count = m_scenePicker.getRecordCount();
    if (count == 0)
    {
        return { nullptr, Vector3::ZERO };
    }

    std::shared_ptr<Enigma::SceneGraph::Pawn> pickedPawn = nullptr;
    Vector3 picked_pos;
    for (unsigned int i = 0; i < count; i++)
    {
        auto pick_record = m_scenePicker.getPickRecord(i);
        if (!pick_record) continue;
        if (pick_record->m_spatial.expired()) continue;
        pickedPawn = std::dynamic_pointer_cast<Enigma::SceneGraph::Pawn, Enigma::SceneGraph::Spatial>(pick_record->m_spatial.lock());
        picked_pos = pick_record->m_vecIntersection;
        if (pickedPawn) break;
    }
    return { pickedPawn, picked_pos };
}