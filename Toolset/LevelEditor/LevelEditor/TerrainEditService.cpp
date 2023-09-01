#include "TerrainEditService.h"
#include "LevelEditorCommands.h"
#include "Terrain/TerrainPrimitiveDto.h"
#include "Terrain/TerrainDtoHelper.h"
#include "Terrain/TerrainGeometryDto.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Terrain/TerrainPawn.h"
#include "Terrain/TerrainPrimitive.h"
#include "Terrain/TerrainGeometry.h"
#include "WorldMap/WorldMapCommands.h"
#include "LevelEditorEvents.h"
#include "MathLib/MathGlobal.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::Terrain;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::WorldMap;

Rtti TerrainEditService::TYPE_RTTI{"LevelEditor.TerrainEditService", &ISystemService::TYPE_RTTI};

std::array<std::string, TerrainEditService::TextureLayerNum> TerrainEditService::LayerSemantics =
{
    "TextureLayer0", "TextureLayer1", "TextureLayer2", "TextureLayer3"
};

const std::string NEW_TERRAIN_TAG = "_new_terrain_";

TerrainEditService::TerrainEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_isHeightMapDirty = false;
    m_dirtyVtxMaxIndex = std::numeric_limits<unsigned int>::min();
    m_dirtyVtxMinIndex = std::numeric_limits<unsigned int>::max();
}

TerrainEditService::~TerrainEditService()
{

}

ServiceResult TerrainEditService::OnInit()
{
    m_doCreatingNewTerrain = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingNewTerrain(c); });
    CommandBus::Subscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_doMovingUpTerrainVertex = std::make_shared<CommandSubscriber>([=](auto c) { DoMovingUpTerrainVertex(c); });
    CommandBus::Subscribe(typeid(LevelEditor::MoveUpTerrainVertex), m_doMovingUpTerrainVertex);
    m_doPaintingTerrainLayer = std::make_shared<CommandSubscriber>([=](auto c) { DoPaintingTerrainLayer(c); });
    CommandBus::Subscribe(typeid(PaintTerrainTextureLayer), m_doPaintingTerrainLayer);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    return ServiceResult::Complete;
}

ServiceResult TerrainEditService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_doCreatingNewTerrain = nullptr;
    CommandBus::Unsubscribe(typeid(LevelEditor::MoveUpTerrainVertex), m_doMovingUpTerrainVertex);
    m_doMovingUpTerrainVertex = nullptr;
    CommandBus::Unsubscribe(typeid(PaintTerrainTextureLayer), m_doPaintingTerrainLayer);
    m_doPaintingTerrainLayer = nullptr;
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;

    return ServiceResult::Complete;
}

void TerrainEditService::MoveUpTerrainVertexByBrush(const Vector3& brush_pos, float brush_size, float height)
{
    assert((height > Math::Epsilon()) || (height < -Math::Epsilon()));
    if (m_pickedTerrain.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->GetPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->GetGeometryData());
    if (!terrain_geo) return;

    m_isHeightMapDirty = true;
    auto cell_dimension = terrain_geo->GetCellDimension();
    float dh = height;
    float sphere_radius = 0.5f * (dh * dh + brush_size * brush_size) / dh;
    int brush_step_x = static_cast<int>(std::ceilf(sphere_radius / cell_dimension.m_width));
    int brush_step_z = static_cast<int>(std::ceilf(sphere_radius / cell_dimension.m_height));
    for (int ix = -brush_step_x; ix <= brush_step_x; ix++)
    {
        for (int iz = -brush_step_z; iz <= brush_step_z; iz++)
        {
            if (ix * ix + iz * iz > brush_step_x * brush_step_z) continue; // diagonal distance over brush radius

            float hh = height;
            float sqr_b = (static_cast<float>(ix) * cell_dimension.m_width * static_cast<float>(ix) * cell_dimension.m_width) +
                (static_cast<float>(iz) * cell_dimension.m_height * static_cast<float>(iz) * cell_dimension.m_height);
            if (sqr_b > sphere_radius * sphere_radius) continue; // distance over brush radius
            float r1 = std::sqrtf(sphere_radius * sphere_radius - sqr_b);
            if (sphere_radius < 0.0f) r1 = -r1;
            hh = height - (sphere_radius - r1);
            Vector3 vtx_pos = Vector3(brush_pos.X() + static_cast<float>(ix) * cell_dimension.m_width,
                brush_pos.Y(),
                brush_pos.Z() + static_cast<float>(iz) * cell_dimension.m_height);
            MoveUpTerrainVertex(terrain_geo, vtx_pos, hh);
        }
    }
    CommitHeightMapUpdated(terrain_prim, terrain_geo);
}

void TerrainEditService::MoveUpTerrainVertex(const std::shared_ptr<TerrainGeometry>& terrain_geometry, const Vector3& picking_pos, float height)
{
    if (m_pickedTerrain.expired()) return;
    if (terrain_geometry == nullptr) return;
    auto cell_dimension = terrain_geometry->GetCellDimension();

    Matrix4 world_inv = m_pickedTerrain.lock()->GetWorldTransform().Inverse();
    Vector3 local_pick_pos = world_inv.TransformCoord(picking_pos);

    auto geo_max_pos = terrain_geometry->GetMaxPosition();
    auto geo_min_pos = terrain_geometry->GetMinPosition();

    unsigned cell_x = (unsigned)((local_pick_pos.X() - geo_min_pos.X()) / cell_dimension.m_width + 0.5f);
    unsigned cell_z = (unsigned)((local_pick_pos.Z() - geo_min_pos.Z()) / cell_dimension.m_height + 0.5f);

    unsigned num_rows = terrain_geometry->GetNumRows();
    unsigned vtxIndex = cell_z * (num_rows + 1) + cell_x;
    terrain_geometry->ChangeHeight(vtxIndex, terrain_geometry->GetHeightMap()[vtxIndex] + height);
    if (m_dirtyVtxMinIndex > vtxIndex) m_dirtyVtxMinIndex = vtxIndex;
    if (m_dirtyVtxMaxIndex < vtxIndex) m_dirtyVtxMaxIndex = vtxIndex;
}

void TerrainEditService::CommitHeightMapUpdated(const std::shared_ptr<TerrainPrimitive>& terrain_primitive,
    const std::shared_ptr<TerrainGeometry>& terrain_geometry)
{
    if (terrain_primitive == nullptr) return;
    if (terrain_geometry == nullptr) return;

    if (m_dirtyVtxMaxIndex < m_dirtyVtxMinIndex) return;

    terrain_geometry->RangedUpdateHeightMapToVertexMemory(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1);
    terrain_primitive->RangedUpdateRenderBuffer(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1, std::nullopt, std::nullopt);
    m_dirtyVtxMaxIndex = std::numeric_limits<unsigned int>::min();
    m_dirtyVtxMinIndex = std::numeric_limits<unsigned int>::max();
}

void TerrainEditService::DoCreatingNewTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateNewTerrain, ICommand>(c);
    if (!cmd) return;

    TerrainPrimitiveDto terrain_dto;
    terrain_dto.Name() = cmd->GetName();
    terrain_dto.TheGeometry() = cmd->GetGeometryDto().ToGenericDto();
    terrain_dto.VisualTechniqueSelection() = "Default";
    EffectMaterialDtoHelper mat_dto("TerrainMesh");
    mat_dto.FilenameAtPath("fx/TerrainMesh.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    for (unsigned i = 0; i < TextureLayerNum; i++)
    {
        if (cmd->GetLayerTextures()[i].empty()) continue;
        tex_dto.TextureMapping(cmd->GetLayerTextures()[i], "APK_PATH", cmd->GetLayerTextures()[i], std::nullopt, LayerSemantics[i]);
    }
    tex_dto.TextureMapping("splat.png", "DataPath", "splat", std::nullopt, "AlphaLayer");
    terrain_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    terrain_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    terrain_dto.GeometryName() = cmd->GetGeometryDto().Name();
    Matrix4 mxLocal = Matrix4::MakeTranslateTransform(cmd->GetLocalPos());
    TerrainPawnDtoHelper pawn_dto(cmd->GetName());
    pawn_dto.TopLevel(true).TerrainPrimitive(terrain_dto).LocalTransform(mxLocal);

    std::vector<GenericDto> dtos = { pawn_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>(NEW_TERRAIN_TAG, dtos));
}

void TerrainEditService::DoMovingUpTerrainVertex(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<LevelEditor::MoveUpTerrainVertex, ICommand>(c);
    if (!cmd) return;
    MoveUpTerrainVertexByBrush(cmd->GetBrushPos(), cmd->GetBrushSize(), cmd->GetBrushHeight());
}

void TerrainEditService::DoPaintingTerrainLayer(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PaintTerrainTextureLayer, ICommand>(c);
    if (!cmd) return;
}

void TerrainEditService::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != NEW_TERRAIN_TAG) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!terrain) return;
    CommandBus::Post(std::make_shared<AttachTerrainToWorldMap>(terrain, terrain->GetLocalTransform()));
}

void TerrainEditService::OnPickedSpatialChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PickedSpatialChanged, IEvent>(e);
    if (!ev) return;
    m_pickedTerrain.reset();
    if (const auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetSpatial())) m_pickedTerrain = terrain;
}
