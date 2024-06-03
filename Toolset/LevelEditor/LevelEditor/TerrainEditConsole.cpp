#include "TerrainEditConsole.h"
#include "LevelEditorAppDelegate.h"
#include "LevelEditorEvents.h"
#include "LevelEditorCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "GameCommon/GameSceneCommands.h"
#include "GameCommon/DeferredRenderingCommands.h"
#include "LevelEditorUiEvents.h"
#include "GeometryDataFileStoreMapper.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Renderer;
using namespace Enigma::SceneGraph;

Rtti TerrainEditConsole::TYPE_RTTI{ "LevelEditor.TerrainEditConsole", &ISystemService::TYPE_RTTI };

std::string BRUSH_SPHERE_TAG = "_brush_sphere_";
std::string SEMANTIC_DECAL_MAP = "DecalMap";

TerrainEditConsole::TerrainEditConsole(ServiceManager* srv_mngr, const std::shared_ptr<GeometryDataFileStoreMapper>& geometry_data_file_store_mapper, const std::string& terrain_path, const std::string& media_path_id) : ISystemService(srv_mngr), m_geometryDataFileStoreMapper(geometry_data_file_store_mapper), m_terrainPath(terrain_path), m_mediaPathId(media_path_id)
{
    m_needTick = false;
    m_isEnabled = false;
    m_currMode = TerrainEditMode::unknown;
    m_brushSize = 1;
    m_brushDensity = 0.0f;
    m_brushHeight = 0.3f;
    m_layerIndex = 0;
}

TerrainEditConsole::~TerrainEditConsole()
{

}

ServiceResult TerrainEditConsole::onInit()
{
    m_onEditorModeChanged = std::make_shared<EventSubscriber>([=](auto e) { onEditorModeChanged(e); });
    EventPublisher::subscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { onPawnPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    m_onTerrainBrushSizeChanged = std::make_shared<EventSubscriber>([=](auto e) { onTerrainBrushSizeChanged(e); });
    EventPublisher::subscribe(typeid(TerrainBrushSizeChanged), m_onTerrainBrushSizeChanged);
    m_onTerrainBrushHeightChanged = std::make_shared<EventSubscriber>([=](auto e) { onTerrainBrushHeightChanged(e); });
    EventPublisher::subscribe(typeid(TerrainBrushHeightChanged), m_onTerrainBrushHeightChanged);
    m_onTerrainBrushDensityChanged = std::make_shared<EventSubscriber>([=](auto e) { onTerrainBrushDensityChanged(e); });
    EventPublisher::subscribe(typeid(TerrainBrushDensityChanged), m_onTerrainBrushDensityChanged);
    m_onTerrainPaintingLayerChanged = std::make_shared<EventSubscriber>([=](auto e) { onTerrainPaintingLayerChanged(e); });
    EventPublisher::subscribe(typeid(TerrainPaintingLayerChanged), m_onTerrainPaintingLayerChanged);
    m_onTerrainToolSelected = std::make_shared<EventSubscriber>([=](auto e) { onTerrainToolSelected(e); });
    EventPublisher::subscribe(typeid(TerrainEditToolSelected), m_onTerrainToolSelected);

    m_onSceneCursorMoved = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorMoved(e); });
    EventPublisher::subscribe(typeid(SceneCursorMoved), m_onSceneCursorMoved);
    m_onSceneCursorDragged = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorDragged(e); });
    EventPublisher::subscribe(typeid(SceneCursorDragged), m_onSceneCursorDragged);
    m_onSceneCursorPressed = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorPressed(e); });
    EventPublisher::subscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorReleased = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorReleased(e); });
    EventPublisher::subscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);

    return ServiceResult::Complete;
}

ServiceResult TerrainEditConsole::onTerm()
{
    EventPublisher::unsubscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onEditorModeChanged = nullptr;
    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    EventPublisher::unsubscribe(typeid(TerrainBrushSizeChanged), m_onTerrainBrushSizeChanged);
    m_onTerrainBrushSizeChanged = nullptr;
    EventPublisher::unsubscribe(typeid(TerrainBrushHeightChanged), m_onTerrainBrushHeightChanged);
    m_onTerrainBrushHeightChanged = nullptr;
    EventPublisher::unsubscribe(typeid(TerrainBrushDensityChanged), m_onTerrainBrushDensityChanged);
    m_onTerrainBrushDensityChanged = nullptr;
    EventPublisher::unsubscribe(typeid(TerrainPaintingLayerChanged), m_onTerrainPaintingLayerChanged);
    m_onTerrainPaintingLayerChanged = nullptr;
    EventPublisher::unsubscribe(typeid(TerrainEditToolSelected), m_onTerrainToolSelected);
    m_onTerrainToolSelected = nullptr;

    EventPublisher::unsubscribe(typeid(SceneCursorMoved), m_onSceneCursorMoved);
    m_onSceneCursorMoved = nullptr;
    EventPublisher::unsubscribe(typeid(SceneCursorDragged), m_onSceneCursorDragged);
    m_onSceneCursorDragged = nullptr;
    EventPublisher::unsubscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorPressed = nullptr;
    EventPublisher::unsubscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);
    m_onSceneCursorReleased = nullptr;

    return ServiceResult::Complete;
}

bool TerrainEditConsole::isTerrainNameDuplicated(const std::string& terrain_name) const
{
    if (m_geometryDataFileStoreMapper.expired()) return false;
    auto terrain_path_name = m_terrainPath + "/" + terrain_name;
    return m_geometryDataFileStoreMapper.lock()->isGeometryNameDuplicated(terrain_path_name);
}

void TerrainEditConsole::createBrushPawn()
{
    /*SphereDtoHelper sphere_helper(BRUSH_SPHERE_TAG);
    sphere_helper.Sphere(Vector3::ZERO, 1.0f, 32, 32).TextureCoord();
    EffectMaterialDtoHelper eff_dto_helper("DeferredShadingDecalGeometry");
    eff_dto_helper.FilenameAtPath("fx/DeferredShadingDecalGeometry.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto_helper;
    tex_dto_helper.TextureMapping("image/terrain_brush_circle.png", "APK_PATH", "terrain_brush_circle", std::nullopt, SEMANTIC_DECAL_MAP);
    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = BRUSH_SPHERE_TAG;
    mesh_dto.GeometryName() = BRUSH_SPHERE_TAG;
    mesh_dto.TheGeometry() = sphere_helper.toGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.toGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto_helper.toGenericDto());
    mesh_dto.RenderListID() = Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(BRUSH_SPHERE_TAG);
    pawn_helper.MeshPrimitive(mesh_dto).TopLevel(true);
    const Matrix4 local = Matrix4::MakeScaleTransform(static_cast<float>(m_brushSize), static_cast<float>(m_brushSize), static_cast<float>(m_brushSize));
    pawn_helper.LocalTransform(local);
    auto dtos = { pawn_helper.toGenericDto() };
    CommandBus::post(std::make_shared<BuildSceneGraph>(BRUSH_SPHERE_TAG, dtos));*/
}

void TerrainEditConsole::sendTerrainEditCommand(float elapse_time)
{
    if (m_brush.expired()) return;
    if (!m_isEnabled) return;
    switch (m_currMode)
    {
    case TerrainEditMode::unknown:
        break;
    case TerrainEditMode::raiseHeight:
        CommandBus::enqueue(std::make_shared<MoveUpTerrainVertex>(m_brush.lock()->getWorldPosition(), static_cast<float>(m_brushSize), elapse_time * m_brushHeight));
        break;
    case TerrainEditMode::lowerHeight:
        CommandBus::enqueue(std::make_shared<MoveUpTerrainVertex>(m_brush.lock()->getWorldPosition(), static_cast<float>(m_brushSize), -elapse_time * m_brushHeight));
        break;
    case TerrainEditMode::paintTexture:
        CommandBus::enqueue(std::make_shared<PaintTerrainTextureLayer>(m_brush.lock()->getWorldPosition(), (float)m_brushSize, m_brushDensity, m_layerIndex));
        break;
    }
}

void TerrainEditConsole::onEditorModeChanged(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EditorModeChanged, IEvent>(e);
    if (!ev) return;
    m_isEnabled = ev->currMode() == EditorMode::terrain;
    if (m_isEnabled)
    {
        CommandBus::enqueue(std::make_shared<OutputMessage>("Mode : Terrain Edit Selected"));
        if (m_brush.expired())
        {
            createBrushPawn();
        }
        else
        {
            m_brush.lock()->removeSpatialFlag(Spatial::SpatialBit::Spatial_Hide);
        }
    }
    else
    {
        if (!m_brush.expired())
        {
            m_brush.lock()->addSpatialFlag(Spatial::SpatialBit::Spatial_Hide);
        }
    }
}

void TerrainEditConsole::onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != BRUSH_SPHERE_TAG) return;
    if (ev->GetTopLevelSpatial().empty()) return;
    auto pawn = std::dynamic_pointer_cast<Pawn, Spatial>(ev->GetTopLevelSpatial().front());
    m_brush = pawn;
    CommandBus::enqueue(std::make_shared<OutputMessage>("Brush Pawn Created"));
    CommandBus::enqueue(std::make_shared<Enigma::GameCommon::AttachSceneRootChild>(pawn, pawn->getLocalTransform()));
}

void TerrainEditConsole::onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (m_brush.expired()) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt>(e);
    if (!ev) return;
    if (ev->pawn() != m_brush.lock()) return;
    CommandBus::enqueue(std::make_shared<Enigma::GameCommon::BindGBuffer>(ev->pawn()));
}

void TerrainEditConsole::onTerrainBrushSizeChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TerrainBrushSizeChanged>(e);
    if (!ev) return;
    m_brushSize = ev->size();
    if (!m_brush.expired())
    {
        m_brush.lock()->setLocalUniformScale(static_cast<float>(m_brushSize));
    }
}

void TerrainEditConsole::onTerrainBrushHeightChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TerrainBrushHeightChanged>(e);
    if (!ev) return;
    m_brushHeight = ev->height();
}

void TerrainEditConsole::onTerrainBrushDensityChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TerrainBrushDensityChanged>(e);
    if (!ev) return;
    m_brushDensity = ev->density();
}

void TerrainEditConsole::onTerrainPaintingLayerChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TerrainPaintingLayerChanged>(e);
    if (!ev) return;
    m_layerIndex = ev->layer();
}

void TerrainEditConsole::onTerrainToolSelected(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TerrainEditToolSelected>(e);
    if (!ev) return;
    switch (ev->tool())
    {
    case TerrainEditToolSelected::Tool::Raise:
        m_currMode = TerrainEditMode::raiseHeight;
        break;
    case TerrainEditToolSelected::Tool::Lower:
        m_currMode = TerrainEditMode::lowerHeight;
        break;
    case TerrainEditToolSelected::Tool::Paint:
        m_currMode = TerrainEditMode::paintTexture;
        break;
    }
    CommandBus::enqueue(std::make_shared<OutputMessage>("Terrain Tool Selected : " + std::to_string(static_cast<int>(m_currMode))));
}

void TerrainEditConsole::onSceneCursorMoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorMoved>(e);
    if (!ev) return;
    if (!m_isEnabled) return;
    if (!ev->hoveredPawn()) return;
    //auto msg = string_format("hover position (%8.3f, %8.3f, %8.3f)\n", ev->GetPosition().X(), ev->GetPosition().Y(), ev->GetPosition().Z());
    //OutputDebugString(msg.c_str());
    if (!m_brush.expired()) m_brush.lock()->changeWorldPosition(ev->position(), std::nullopt);
}

void TerrainEditConsole::onSceneCursorDragged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorDragged>(e);
    if (!ev) return;
    if (!m_isEnabled) return;
    if (!ev->hoveredPawn()) return;
    //auto msg = string_format("hover position (%8.3f, %8.3f, %8.3f)\n", ev->GetPosition().X(), ev->GetPosition().Y(), ev->GetPosition().Z());
    //OutputDebugString(msg.c_str());
    if (!m_brush.expired()) m_brush.lock()->changeWorldPosition(ev->position(), std::nullopt);
    sendTerrainEditCommand(0.1f);
}

void TerrainEditConsole::onSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorPressed>(e);
    if (!ev) return;
    if (!m_isEnabled) return;
    if (!ev->pickedPawn()) return;
    if (!m_brush.expired()) m_brush.lock()->changeWorldPosition(ev->position(), std::nullopt);
    sendTerrainEditCommand(1.0f);
}

void TerrainEditConsole::onSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorReleased>(e);
    if (!ev) return;
    if (!m_isEnabled) return;
    if (!ev->pickedPawn()) return;
    if (!m_brush.expired())
    {
        m_brush.lock()->changeWorldPosition(ev->position(), std::nullopt);
        CommandBus::enqueue(std::make_shared<CompleteTerrainEditOperation>(m_brush.lock()->getWorldPosition()));
    }
}
