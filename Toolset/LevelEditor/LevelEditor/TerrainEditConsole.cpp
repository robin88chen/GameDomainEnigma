#include "TerrainEditConsole.h"
#include "LevelEditorEvents.h"
#include "LevelEditorCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "GameCommon/GameSceneCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Renderer;
using namespace Enigma::SceneGraph;

Rtti TerrainEditConsole::TYPE_RTTI{"LevelEditor.TerrainEditConsole", &ISystemService::TYPE_RTTI};

std::string BRUSH_SPHERE_TAG = "_brush_sphere_";
std::string SEMANTIC_DECAL_MAP = "DecalMap";

TerrainEditConsole::TerrainEditConsole(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_isEnabled = false;
}

TerrainEditConsole::~TerrainEditConsole()
{

}

ServiceResult TerrainEditConsole::OnInit()
{
    m_onEditorModeChanged = std::make_shared<EventSubscriber>([=](auto e) { OnEditorModeChanged(e); });
    EventPublisher::Subscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    return ServiceResult::Complete;
}

ServiceResult TerrainEditConsole::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onEditorModeChanged = nullptr;
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    return ServiceResult::Complete;
}

void TerrainEditConsole::CreateBrushPawn()
{
    SphereDtoHelper sphere_helper(BRUSH_SPHERE_TAG);
    sphere_helper.Sphere(Vector3::ZERO, 1.0f, 32, 32).TextureCoord();
    EffectMaterialDtoHelper eff_dto_helper("DeferredShadingDecalGeometry");
    eff_dto_helper.FilenameAtPath("fx/DeferredShadingDecalGeometry.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto_helper;
    tex_dto_helper.TextureMapping("image/terrain_brush_circle.png", "APK_PATH", "terrain_brush_circle", std::nullopt, SEMANTIC_DECAL_MAP);
    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = BRUSH_SPHERE_TAG;
    mesh_dto.GeometryName() = BRUSH_SPHERE_TAG;
    mesh_dto.TheGeometry() = sphere_helper.ToGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.ToGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto_helper.ToGenericDto());
    mesh_dto.RenderListID() = Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(BRUSH_SPHERE_TAG);
    pawn_helper.MeshPrimitive(mesh_dto)
        .SpatialFlags(Spatial::Spatial_BelongToParent).TopLevel(true);
    auto dtos = { pawn_helper.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>(BRUSH_SPHERE_TAG, dtos));
}

void TerrainEditConsole::OnEditorModeChanged(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EditorModeChanged, IEvent>(e);
    if (!ev) return;
    m_isEnabled = ev->GetCurrMode() == EditorMode::Terrain;
    if (m_isEnabled)
    {
        CommandBus::Post(std::make_shared<OutputMessage>("Mode : Terrain Edit Selected"));
        CreateBrushPawn();
    }
}

void TerrainEditConsole::OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != BRUSH_SPHERE_TAG) return;
    if (ev->GetTopLevelSpatial().empty()) return;
    auto pawn = std::dynamic_pointer_cast<Pawn, Spatial>(ev->GetTopLevelSpatial().front());
    m_brush = pawn;
    CommandBus::Post(std::make_shared<OutputMessage>("Brush Pawn Created"));
    CommandBus::Post(std::make_shared<Enigma::GameCommon::AttachSceneRootChild>(pawn, Enigma::MathLib::Matrix4::IDENTITY));
}
