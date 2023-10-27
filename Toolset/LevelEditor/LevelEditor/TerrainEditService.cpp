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
#include "GameEngine/Texture.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "Frameworks/StringFormat.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::Terrain;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::WorldMap;
using namespace Enigma::FileSystem;

Rtti TerrainEditService::TYPE_RTTI{ "LevelEditor.TerrainEditService", &ISystemService::TYPE_RTTI };

std::array<std::string, TerrainEditService::TextureLayerNum> TerrainEditService::LayerSemantics =
{
    "TextureLayer0", "TextureLayer1", "TextureLayer2", "TextureLayer3"
};

const std::string NEW_TERRAIN_TAG = "_new_terrain_";
const std::string SPLAT_TEXTURE_POSTFIX = "_splat";
const std::string ALPHA_TEXTURE_SEMANTIC = "AlphaLayer";

TerrainEditService::TerrainEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_isHeightMapDirty = false;
    m_dirtyVtxMaxIndex = std::numeric_limits<unsigned int>::min();
    m_dirtyVtxMinIndex = std::numeric_limits<unsigned int>::max();
    m_dirtyAlphaRect.Left() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Right() = std::numeric_limits<int>::min();
    m_dirtyAlphaRect.Top() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Bottom() = std::numeric_limits<int>::min();
    m_alphaRect = { 0, 0, 0, 0 };
}

TerrainEditService::~TerrainEditService()
{

}

ServiceResult TerrainEditService::onInit()
{
    m_doCreatingNewTerrain = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingNewTerrain(c); });
    CommandBus::subscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_doMovingUpTerrainVertex = std::make_shared<CommandSubscriber>([=](auto c) { DoMovingUpTerrainVertex(c); });
    CommandBus::subscribe(typeid(LevelEditor::MoveUpTerrainVertex), m_doMovingUpTerrainVertex);
    m_doPaintingTerrainLayer = std::make_shared<CommandSubscriber>([=](auto c) { DoPaintingTerrainLayer(c); });
    CommandBus::subscribe(typeid(PaintTerrainTextureLayer), m_doPaintingTerrainLayer);
    m_doSavingSplatTexture = std::make_shared<CommandSubscriber>([=](auto c) { DoSavingSplatTexture(c); });
    CommandBus::subscribe(typeid(SaveTerrainSplatTexture), m_doSavingSplatTexture);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onTerrainPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnTerrainPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onTerrainPrimitiveBuilt);
    m_onPickedSpatialChanged = std::make_shared<EventSubscriber>([=](auto e) { OnPickedSpatialChanged(e); });
    EventPublisher::subscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);

    m_onSplatTextureSaved = std::make_shared<EventSubscriber>([=](auto e) { OnSplatTextureSaved(e); });
    EventPublisher::subscribe(typeid(TextureSaved), m_onSplatTextureSaved);
    m_onSaveSplatTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { OnSaveSplatTextureFailed(e); });
    EventPublisher::subscribe(typeid(SaveTextureFailed), m_onSaveSplatTextureFailed);
    m_onTextureImageRetrieved = std::make_shared<EventSubscriber>([=](auto e) { OnTextureImageRetrieved(e); });
    EventPublisher::subscribe(typeid(TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onRetrieveTextureImageFailed = std::make_shared<EventSubscriber>([=](auto e) { OnRetrieveTextureImageFailed(e); });
    EventPublisher::subscribe(typeid(RetrieveTextureImageFailed), m_onRetrieveTextureImageFailed);

    return ServiceResult::Complete;
}

ServiceResult TerrainEditService::onTerm()
{
    CommandBus::unsubscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_doCreatingNewTerrain = nullptr;
    CommandBus::unsubscribe(typeid(LevelEditor::MoveUpTerrainVertex), m_doMovingUpTerrainVertex);
    m_doMovingUpTerrainVertex = nullptr;
    CommandBus::unsubscribe(typeid(PaintTerrainTextureLayer), m_doPaintingTerrainLayer);
    m_doPaintingTerrainLayer = nullptr;
    CommandBus::unsubscribe(typeid(SaveTerrainSplatTexture), m_doSavingSplatTexture);
    m_doSavingSplatTexture = nullptr;

    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onTerrainPrimitiveBuilt);
    m_onTerrainPrimitiveBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
    m_onPickedSpatialChanged = nullptr;

    EventPublisher::unsubscribe(typeid(TextureSaved), m_onSplatTextureSaved);
    m_onSplatTextureSaved = nullptr;
    EventPublisher::unsubscribe(typeid(SaveTextureFailed), m_onSaveSplatTextureFailed);
    m_onSaveSplatTextureFailed = nullptr;
    EventPublisher::unsubscribe(typeid(TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onTextureImageRetrieved = nullptr;
    EventPublisher::unsubscribe(typeid(RetrieveTextureImageFailed), m_onRetrieveTextureImageFailed);
    m_onRetrieveTextureImageFailed = nullptr;

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
    // use ellipsoid to calculate heights
    // x^2 / a^2 + y^2 / b^2 + z^2 / c^2 = 1
    float sqr_a = brush_size * brush_size;
    float sqr_b = height * height;
    float sqr_c = brush_size * brush_size;
    int brush_step_x = static_cast<int>(std::floorf(brush_size / cell_dimension.m_width));
    int brush_step_z = static_cast<int>(std::floorf(brush_size / cell_dimension.m_height));
    for (int ix = -brush_step_x; ix <= brush_step_x; ix++)
    {
        for (int iz = -brush_step_z; iz <= brush_step_z; iz++)
        {
            float x = static_cast<float>(ix) * cell_dimension.m_width;
            float z = static_cast<float>(iz) * cell_dimension.m_height;
            float sqr_xz = x * x / sqr_a + z * z / sqr_c;
            if (sqr_xz > 1.0f) continue; // outside of ellipsoid
            float sqr_y = sqr_b * (1.0f - sqr_xz);
            float y = std::sqrtf(sqr_y);
            if (height < 0.0f) y = -y;
            Vector3 vtx_pos = Vector3(brush_pos.X() + x, brush_pos.Y(), brush_pos.Z() + z);
            MoveUpTerrainVertex(terrain_geo, vtx_pos, y);
        }
    }
    CommitHeightMapUpdated(terrain_prim, terrain_geo);
}

void TerrainEditService::MoveUpTerrainVertex(const std::shared_ptr<TerrainGeometry>& terrain_geometry, const Vector3& picking_pos, float height)
{
    if (m_pickedTerrain.expired()) return;
    if (terrain_geometry == nullptr) return;

    Matrix4 world_inv = m_pickedTerrain.lock()->GetWorldTransform().Inverse();
    Vector3 local_pick_pos = world_inv.TransformCoord(picking_pos);

    auto [cell_x, cell_z] = terrain_geometry->LocateCell(local_pick_pos);

    terrain_geometry->ChangeHeight(cell_x, cell_z, terrain_geometry->GetHeight(cell_x, cell_z) + height);
    auto vtx_idx = terrain_geometry->ConvertVertexIndex(cell_x, cell_z);
    if (m_dirtyVtxMinIndex > vtx_idx) m_dirtyVtxMinIndex = vtx_idx;
    if (m_dirtyVtxMaxIndex < vtx_idx) m_dirtyVtxMaxIndex = vtx_idx;
}

void TerrainEditService::CommitHeightMapUpdated(const std::shared_ptr<TerrainPrimitive>& terrain_primitive,
    const std::shared_ptr<TerrainGeometry>& terrain_geometry)
{
    if (terrain_primitive == nullptr) return;
    if (terrain_geometry == nullptr) return;

    if (m_dirtyVtxMaxIndex < m_dirtyVtxMinIndex) return;

    terrain_geometry->RangedUpdateHeightMapToVertexMemory(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1);
    terrain_geometry->RangedUpdateVertexNormals(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1);
    terrain_primitive->RangedUpdateRenderBuffer(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1, std::nullopt, std::nullopt);
    m_dirtyVtxMaxIndex = std::numeric_limits<unsigned int>::min();
    m_dirtyVtxMinIndex = std::numeric_limits<unsigned int>::max();
}

void TerrainEditService::PaintTerrainLayerByBrush(const Vector3& brush_pos, float brush_size, unsigned layer_idx, float density)
{
    assert(layer_idx < TextureLayerNum);
    if (m_pickedTerrain.expired()) return;
    if (m_pickedSplatTexture.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->GetPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->GetGeometryData());
    if (!terrain_geo) return;

    auto cell_dimension = terrain_geo->GetCellDimension();
    // use ellipsoid to calculate heights
    // x^2 / a^2 + y^2 / b^2 + z^2 / c^2 = 1
    float sqr_a = brush_size * brush_size;
    float sqr_b = density * density;
    float sqr_c = brush_size * brush_size;
    int brush_step_x = static_cast<int>(std::floorf(brush_size / cell_dimension.m_width));
    int brush_step_z = static_cast<int>(std::floorf(brush_size / cell_dimension.m_height));
    for (int ix = -brush_step_x; ix <= brush_step_x; ix++)
    {
        for (int iz = -brush_step_z; iz <= brush_step_z; iz++)
        {
            float x = static_cast<float>(ix) * cell_dimension.m_width;
            float z = static_cast<float>(iz) * cell_dimension.m_height;
            float sqr_xz = x * x / sqr_a + z * z / sqr_c;
            if (sqr_xz > 1.0f) continue; // outside of ellipsoid
            float sqr_y = sqr_b * (1.0f - sqr_xz);
            float y = std::sqrtf(sqr_y);
            Vector3 vtx_pos = Vector3(brush_pos.X() + x, brush_pos.Y(), brush_pos.Z() + z);
            PaintTerrainLayer(vtx_pos, layer_idx, y);
        }
    }
    CommitAlphaTexelUpdated();
}

void TerrainEditService::PaintTerrainLayer(const Vector3& picking_pos, unsigned layer_idx, float density)
{
    assert(layer_idx < TextureLayerNum);
    if (m_pickedTerrain.expired()) return;
    if (m_pickedSplatTexture.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->GetPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->GetGeometryData());
    if (!terrain_geo) return;

    Matrix4 world_inv = m_pickedTerrain.lock()->GetWorldTransform().Inverse();
    Vector3 local_pick_pos = world_inv.TransformCoord(picking_pos);
    auto min_local_pos = terrain_geo->GetMinPosition();

    auto num_cols = terrain_geo->GetNumCols();
    auto num_rows = terrain_geo->GetNumRows();
    auto cell_dimension = terrain_geo->GetCellDimension();
    auto tex_dimension = m_pickedSplatTexture.lock()->GetDimension();
    auto texel_per_cell_x = tex_dimension.m_width / num_cols;
    auto texel_per_cell_z = tex_dimension.m_height / num_rows;
    float texel_grid_size_x = cell_dimension.m_width / static_cast<float>(texel_per_cell_x);
    float texel_grid_size_z = cell_dimension.m_height / static_cast<float>(texel_per_cell_z);
    auto texel_x = static_cast<unsigned>((local_pick_pos.X() - min_local_pos.X()) / texel_grid_size_x);
    if (texel_x >= tex_dimension.m_width) return;
    auto texel_z = static_cast<unsigned>((local_pick_pos.Z() - min_local_pos.Z()) / texel_grid_size_z);
    if (texel_z >= tex_dimension.m_height) return;
    texel_z = (tex_dimension.m_height - 1) - texel_z;  // invert texel z, bcz texcoord is y-down
    if (m_dirtyAlphaRect.Left() > static_cast<int>(texel_x)) m_dirtyAlphaRect.Left() = static_cast<int>(texel_x);
    if (m_dirtyAlphaRect.Right() < static_cast<int>(texel_x + 1)) m_dirtyAlphaRect.Right() = static_cast<int>(texel_x + 1);
    if (m_dirtyAlphaRect.Top() > static_cast<int>(texel_z)) m_dirtyAlphaRect.Top() = static_cast<int>(texel_z);
    if (m_dirtyAlphaRect.Bottom() < static_cast<int>(texel_z + 1)) m_dirtyAlphaRect.Bottom() = static_cast<int>(texel_z + 1);
    AddLayerAlpha(texel_x, texel_z, layer_idx, static_cast<int>(density * 255));
}

void TerrainEditService::AddLayerAlpha(unsigned texel_x, unsigned texel_y, unsigned layer_idx, int density)
{
    if (m_alphaTexels.empty()) return;
    if (m_pickedSplatTexture.expired()) return;

    auto dimension = m_pickedSplatTexture.lock()->GetDimension();
    unsigned int texel_base_index = (texel_y * dimension.m_width + texel_x) * TextureLayerNum;
    if (layer_idx == 0)
    {
        m_alphaTexels[texel_base_index] = 0;
        m_alphaTexels[texel_base_index + 1] = 0;
        m_alphaTexels[texel_base_index + 2] = 0;
        m_alphaTexels[texel_base_index + 3] = 0;
    }
    else
    {
        unsigned int index = texel_base_index + layer_idx - 1;
        if (index >= m_alphaTexels.size()) return;
        int src_alpha = (int)(m_alphaTexels[index]);
        src_alpha += density;
        if (src_alpha < 0) src_alpha = 0;
        if (src_alpha > 255) src_alpha = 255;
        m_alphaTexels[index] = static_cast<unsigned char>(src_alpha);
    }
}

void TerrainEditService::CommitAlphaTexelUpdated()
{
    if (m_pickedSplatTexture.expired()) return;
    if (m_alphaTexels.empty()) return;
    if (m_dirtyAlphaRect.Left() > m_dirtyAlphaRect.Right()) return;
    if (m_dirtyAlphaRect.Top() > m_dirtyAlphaRect.Bottom()) return;
    unsigned int start_x = m_dirtyAlphaRect.Left();
    unsigned int width = m_dirtyAlphaRect.Width();
    unsigned int start_z = m_dirtyAlphaRect.Top();
    unsigned int height = m_dirtyAlphaRect.Height();
    m_dirtyAlphaTexels.resize(width * height * TextureLayerNum);
    for (unsigned int z = start_z; z < start_z + height; z++)
    {
        unsigned int src_idx = (z * m_alphaRect.Width() + start_x) * TextureLayerNum;
        unsigned int dest_idx = (z - start_z) * width * TextureLayerNum;
        std::memcpy(&m_dirtyAlphaTexels[dest_idx], &m_alphaTexels[src_idx],
            width * TextureLayerNum);
    }
    CommandBus::post(std::make_shared<UpdateTextureImage>(m_pickedSplatTexture.lock(), m_pickedSplatTexture.lock()->getName(), m_dirtyAlphaRect, m_dirtyAlphaTexels));
    m_dirtyAlphaRect.Left() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Right() = std::numeric_limits<int>::min();
    m_dirtyAlphaRect.Top() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Bottom() = std::numeric_limits<int>::min();
}

void TerrainEditService::DoCreatingNewTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateNewTerrain, ICommand>(c);
    if (!cmd) return;
    m_terrainPathId = cmd->GetAssetPathId();

    TerrainPrimitiveDto terrain_dto;
    terrain_dto.Name() = cmd->getName();
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
    auto splat_texture_name = cmd->getName() + SPLAT_TEXTURE_POSTFIX;
    tex_dto.TextureMapping(Enigma::MathLib::Dimension<unsigned>{512, 512}, 1, splat_texture_name, std::nullopt, ALPHA_TEXTURE_SEMANTIC);
    terrain_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    terrain_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    terrain_dto.GeometryName() = cmd->GetGeometryDto().Name();
    Matrix4 mxLocal = Matrix4::MakeTranslateTransform(cmd->GetLocalPos());
    TerrainPawnDtoHelper pawn_dto(cmd->getName());
    pawn_dto.TopLevel(true).TerrainPrimitive(terrain_dto).LocalTransform(mxLocal);

    std::vector<GenericDto> dtos = { pawn_dto.ToGenericDto() };
    CommandBus::post(std::make_shared<BuildSceneGraph>(NEW_TERRAIN_TAG, dtos));
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
    PaintTerrainLayerByBrush(cmd->GetBrushPos(), cmd->GetBrushSize(), cmd->GetTextureLayerIndex(), cmd->GetBrushDensity());
}

void TerrainEditService::DoCompletingEditOperation(const ICommandPtr& c)
{

}

void TerrainEditService::DoSavingSplatTexture(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<SaveTerrainSplatTexture, ICommand>(c);
    if (!cmd) return;
    if (m_savingSplatTextureFile)
    {
        CommandBus::post(std::make_shared<OutputMessage>("Save Splat Texture : previous saving not complete yet"));
        return;
    }
    if (m_pickedSplatTexture.expired())
    {
        CommandBus::post(std::make_shared<OutputMessage>("Save Splat Texture : No splat texture selected"));
        return;
    }
    if (m_pickedTerrain.expired())
    {
        CommandBus::post(std::make_shared<OutputMessage>("Save Splat Texture : No terrain selected"));
        return;
    }
    m_savingSplatTextureFile = FileSystem::Instance()->OpenFile(m_pickedSplatTexture.lock()->TheFactoryDesc().GetResourceFilename(), Write | OpenAlways | Binary);
    CommandBus::post(std::make_shared<SaveTexture>(m_pickedSplatTexture.lock(), m_pickedSplatTexture.lock()->getName(), m_savingSplatTextureFile));
}

void TerrainEditService::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != NEW_TERRAIN_TAG) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!terrain) return;
    terrain->TheFactoryDesc().ClaimAsInstanced(terrain->GetSpatialName() + ".pawn", m_terrainPathId);
    CommandBus::post(std::make_shared<AttachTerrainToWorldMap>(terrain, terrain->GetLocalTransform()));
}

void TerrainEditService::OnTerrainPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn>(ev->GetPawn());
    if (!terrain) return;
    auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(terrain->GetPrimitive());
    if (!terrain_prim) return;
    terrain_prim->TheFactoryDesc().ClaimAsInstanced(terrain_prim->getName() + ".terrain", m_terrainPathId);
    auto splat_texture = terrain_prim->FindTextureBySemantic(ALPHA_TEXTURE_SEMANTIC);
    if (!splat_texture) return;
    std::string splat_texture_resouce_name = splat_texture->getName();
    splat_texture->TheFactoryDesc().ClaimAsResourceAsset(splat_texture_resouce_name, splat_texture_resouce_name + ".png", m_terrainPathId);
    m_splatTextures.insert_or_assign(terrain->GetSpatialName(), splat_texture);
}

void TerrainEditService::OnPickedSpatialChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PickedSpatialChanged, IEvent>(e);
    if (!ev) return;
    m_pickedTerrain.reset();
    m_pickedSplatTexture.reset();
    if (const auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetSpatial())) m_pickedTerrain = terrain;
    if (m_pickedTerrain.expired()) return;
    if (const auto splat_texture = m_splatTextures.find(m_pickedTerrain.lock()->GetSpatialName()); splat_texture != m_splatTextures.end())
    {
        m_pickedSplatTexture = splat_texture->second;
    }
    if (m_pickedSplatTexture.expired()) return;
    m_alphaRect = { 0, 0, static_cast<int>(m_pickedSplatTexture.lock()->GetDimension().m_width), static_cast<int>(m_pickedSplatTexture.lock()->GetDimension().m_height) };
    CommandBus::post(std::make_shared<RetrieveTextureImage>(m_pickedSplatTexture.lock(), m_pickedSplatTexture.lock()->getName(), m_alphaRect));
}

void TerrainEditService::OnTextureImageRetrieved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageRetrieved, IEvent>(e);
    if (!ev) return;
    if (m_pickedSplatTexture.expired()) return;
    if (ev->getName() != m_pickedSplatTexture.lock()->getName()) return;
    m_alphaTexels = ev->GetImageBuffer();
}

void TerrainEditService::OnRetrieveTextureImageFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RetrieveTextureImageFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetErrorCode())
    {
        CommandBus::post(std::make_shared<OutputMessage>(string_format("Retrieve splat texture %s image failed %s", ev->getName().c_str(), ev->GetErrorCode().message().c_str())));
    }
}

void TerrainEditService::OnSplatTextureSaved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureSaved, IEvent>(e);
    if (!ev) return;
    if (!m_savingSplatTextureFile) return;
    if (m_pickedSplatTexture.expired())
    {
        FileSystem::Instance()->CloseFile(m_savingSplatTextureFile);
        m_savingSplatTextureFile = nullptr;
        return;
    }
    if (ev->getName() != m_pickedSplatTexture.lock()->getName()) return;
    FileSystem::Instance()->CloseFile(m_savingSplatTextureFile);
    m_savingSplatTextureFile = nullptr;
}

void TerrainEditService::OnSaveSplatTextureFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SaveTextureFailed, IEvent>(e);
    if (!ev) return;
    if (!m_savingSplatTextureFile) return;
    if (m_pickedSplatTexture.expired())
    {
        FileSystem::Instance()->CloseFile(m_savingSplatTextureFile);
        m_savingSplatTextureFile = nullptr;
        return;
    }
    if (ev->getName() != m_pickedSplatTexture.lock()->getName()) return;
    FileSystem::Instance()->CloseFile(m_savingSplatTextureFile);
    m_savingSplatTextureFile = nullptr;
    CommandBus::post(std::make_shared<OutputMessage>(string_format("Save splat texture %s image failed %s", ev->getName().c_str(), ev->GetErrorCode().message().c_str())));
}
