#include "EditorSceneConsole.h"
#include "SceneGraph/OutRegionNode.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/PortalCommands.h"
#include "WorldMap/WorldMap.h"
#include "WorldMap/WorldMapQueries.h"
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
#include "LevelEditorCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::InputHandlers;
using namespace Enigma::Renderer;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::WorldMap;

Rtti EditorSceneConsole::TYPE_RTTI{ "LevelEditor.EditorSceneConsole", &ISystemService::TYPE_RTTI };

EditorSceneConsole::EditorSceneConsole(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
}

EditorSceneConsole::~EditorSceneConsole()
{
}

ServiceResult EditorSceneConsole::onInit()
{
    m_onGameCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { onGameCameraCreated(e); });
    EventPublisher::subscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    m_onSceneRootCreated = std::make_shared<EventSubscriber>([=](auto e) { onSceneRootCreated(e); });
    EventPublisher::subscribe(typeid(PortalSceneRootCreated), m_onSceneRootCreated);
    m_onTargetViewportChanged = std::make_shared<EventSubscriber>([=](auto e) { onTargetViewportChanged(e); });
    EventPublisher::subscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onPickedSpatialChanged = std::make_shared<EventSubscriber>([=](auto e) { onPickedSpatialChanged(e); });
    EventPublisher::subscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
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

    m_dropAssetToSceneGraph = std::make_shared<CommandSubscriber>([=](auto c) { dropAssetToSceneGraph(c); });
    CommandBus::subscribe(typeid(DropAssetToSceneGraph), m_dropAssetToSceneGraph);

    return ServiceResult::Complete;
}

ServiceResult EditorSceneConsole::onTerm()
{
    EventPublisher::unsubscribe(typeid(GameCameraCreated), m_onGameCameraCreated);
    m_onGameCameraCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PortalSceneRootCreated), m_onSceneRootCreated);
    m_onSceneRootCreated = nullptr;
    EventPublisher::unsubscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onTargetViewportChanged = nullptr;
    EventPublisher::unsubscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
    m_onPickedSpatialChanged = nullptr;
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

    CommandBus::unsubscribe(typeid(DropAssetToSceneGraph), m_dropAssetToSceneGraph);
    m_dropAssetToSceneGraph = nullptr;

    return ServiceResult::Complete;
}

void EditorSceneConsole::onGameCameraCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GameCameraCreated, IEvent>(e);
    if (!ev) return;
    m_camera = ev->GetCamera();
}

void EditorSceneConsole::onSceneRootCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<PortalSceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->root();
}

void EditorSceneConsole::onTargetViewportChanged(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TargetViewPortChanged, IEvent>(e);
    if (!ev) return;
    m_targetViewport = ev->viewPort();
}

void EditorSceneConsole::onPickedSpatialChanged(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<PickedSpatialChanged, IEvent>(e);
    if (!ev) return;
    if (ev->wherePickedFrom() != PickedSpatialChanged::PickedFrom::FromSceneGraph) return;
    m_pickedSpatialId = ev->id();
}

void EditorSceneConsole::onMouseMoved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseMoved>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::enqueue(std::make_shared<SceneCursorMoved>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftButtonDown(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDown>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::enqueue(std::make_shared<SceneCursorPressed>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftButtonUp(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonUp>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::enqueue(std::make_shared<SceneCursorReleased>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onMouseLeftDragged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseLeftButtonDrag>(e);
    if (!ev) return;
    auto clipping_pos = m_targetViewport.ViewportPositionToClippingPosition(
        Vector2(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y)));
    auto [pickedPawn, picked_pos] = pickingOnSceneView(clipping_pos);
    if (pickedPawn)
    {
        EventPublisher::enqueue(std::make_shared<SceneCursorDragged>(picked_pos, pickedPawn, ev->m_param));
    }
}

void EditorSceneConsole::onKeyboardAsyncKeyPressed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<WinKeyboardAsyncPressed>(e);
    if (!ev) return;
    if (ev->m_param.m_virtualKey == 'A')
    {
        CommandBus::enqueue(std::make_shared<MoveCamera>(0.0f, -0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'D')
    {
        CommandBus::enqueue(std::make_shared<MoveCamera>(0.0f, 0.1f));
    }
    else if (ev->m_param.m_virtualKey == 'S')
    {
        CommandBus::enqueue(std::make_shared<MoveCamera>(-0.1f, 0.0f));
    }
    else if (ev->m_param.m_virtualKey == 'W')
    {
        CommandBus::enqueue(std::make_shared<MoveCamera>(0.1f, 0.0f));
    }
}

void EditorSceneConsole::dropAssetToSceneGraph(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DropAssetToSceneGraph>(c);
    if (!cmd) return;
    AssetIdCombo asset_id = cmd->assetId();
    SpatialId scene_graph_id = cmd->targetId();
    if (asset_id.isSpatialId())
    {
        if (asset_id.getSpatialId() == scene_graph_id)
        {
            CommandBus::enqueue(std::make_shared<OutputMessage>("Cannot Drop Spatial Asset to Itself"));
            return;
        }
        CommandBus::enqueue(std::make_shared<OutputMessage>("Drop Spatial Asset " + asset_id.getSpatialId().name() + " to Scene Graph " + scene_graph_id.name()));
        std::shared_ptr<Spatial> child = Spatial::querySpatial(asset_id.getSpatialId());
        if ((child) && (!child->getParentId()))
        {
            CommandBus::enqueue(std::make_shared<AttachNodeChild>(scene_graph_id, child, child->getLocalTransform()));
        }
    }
    else if (asset_id.isWorldMapId())
    {
        std::shared_ptr<WorldMap> world_map = std::make_shared<QueryWorldMap>(asset_id.getWorldMapId())->dispatch();
        assert(world_map);
        if (world_map->outRegionId().empty())
        {
            CommandBus::enqueue(std::make_shared<OutputMessage>("World Map Asset " + asset_id.getWorldMapId().name() + " has no Region"));
            return;
        }
        if (!scene_graph_id.rtti().isDerived(PortalManagementNode::TYPE_RTTI))
        {
            CommandBus::enqueue(std::make_shared<OutputMessage>("Must Drop World Map Asset to Portal Management Node"));
            return;
        }
        CommandBus::enqueue(std::make_shared<OutputMessage>("Drop World Map Asset " + asset_id.getWorldMapId().name() + " to Scene Graph " + scene_graph_id.name()));
        std::shared_ptr<OutRegionNode> child = std::dynamic_pointer_cast<OutRegionNode>(Spatial::querySpatial(world_map->outRegionId()));
        if ((child) && (!child->ownerManagementNode()))
        {
            CommandBus::enqueue(std::make_shared<AttachManagementOutsideRegion>(child));
        }
    }
}

std::tuple<std::shared_ptr<Pawn>, Vector3> EditorSceneConsole::pickingOnSceneView(const Vector2& clip_pos)
{
    if (m_camera.expired()) return { nullptr, Vector3::ZERO };
    if (m_sceneRoot.expired()) return { nullptr, Vector3::ZERO };

    m_scenePicker.setAssociatedCamera(m_camera.lock());
    m_scenePicker.setClippingCoord(clip_pos);
    m_scenePicker.picking(m_sceneRoot.lock());

    const unsigned int count = m_scenePicker.getRecordCount();
    if (count == 0)
    {
        return { nullptr, Vector3::ZERO };
    }

    std::shared_ptr<Pawn> pickedPawn = nullptr;
    Vector3 picked_pos;
    for (unsigned int i = 0; i < count; i++)
    {
        auto pick_record = m_scenePicker.getPickRecord(i);
        if (!pick_record) continue;
        if (pick_record->m_spatial.expired()) continue;
        pickedPawn = std::dynamic_pointer_cast<Pawn, Spatial>(pick_record->m_spatial.lock());
        picked_pos = pick_record->m_vecIntersection;
        if (pickedPawn) break;
    }
    return { pickedPawn, picked_pos };
}
