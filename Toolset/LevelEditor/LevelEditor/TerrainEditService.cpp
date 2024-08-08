#include "TerrainEditService.h"
#include "Terrain/TerrainAssemblers.h"
#include "LevelEditorCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Terrain/TerrainPawn.h"
#include "Terrain/TerrainPrimitive.h"
#include "Terrain/TerrainGeometry.h"
#include "Geometries/GeometryDataQueries.h"
#include "Geometries/GeometryCommands.h"
#include "WorldMap/WorldMapCommands.h"
#include "LevelEditorEvents.h"
#include "MathLib/MathGlobal.h"
#include "GameEngine/Texture.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "Frameworks/StringFormat.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "GameEngine/TextureDto.h"
#include "Primitives/PrimitiveQueries.h"
#include "Primitives/PrimitiveCommands.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "GameCommon/GameSceneCommands.h"
#include "GameEngine/TextureQueries.h"

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

TerrainEditService::TerrainEditService(ServiceManager* srv_mngr, const std::shared_ptr<Enigma::FileStorage::TextureFileStoreMapper>& texture_file_store_mapper) : ISystemService(srv_mngr), m_textureFileStoreMapper(texture_file_store_mapper)
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
    m_createNewTerrain = std::make_shared<CommandSubscriber>([=](auto c) { createNewTerrain(c); });
    CommandBus::subscribe(typeid(CreateNewTerrain), m_createNewTerrain);
    m_moveUpTerrainVertex = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { moveUpTerrainVertex(c); });
    CommandBus::subscribe(typeid(MoveUpTerrainVertex), m_moveUpTerrainVertex);
    m_paintTerrainLayer = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { paintTerrainLayer(c); });
    CommandBus::subscribe(typeid(PaintTerrainTextureLayer), m_paintTerrainLayer);
    m_saveSplatTexture = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { saveSplatTexture(c); });
    CommandBus::subscribe(typeid(SaveTerrainSplatTexture), m_saveSplatTexture);
    m_savePickedTerrain = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { savePickedTerrain(c); });
    CommandBus::subscribe(typeid(SavePickedTerrain), m_savePickedTerrain);

    //m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    //EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    //m_onTerrainPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { onTerrainPrimitiveBuilt(e); });
    //EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onTerrainPrimitiveBuilt);
    m_onPickedSpatialChanged = std::make_shared<EventSubscriber>([=](auto e) { onPickedSpatialChanged(e); });
    EventPublisher::subscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);

    m_onSplatTextureSaved = std::make_shared<EventSubscriber>([=](auto e) { onSplatTextureSaved(e); });
    EventPublisher::subscribe(typeid(TextureSaved), m_onSplatTextureSaved);
    m_onSaveSplatTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { onSaveSplatTextureFailed(e); });
    EventPublisher::subscribe(typeid(SaveTextureFailed), m_onSaveSplatTextureFailed);
    m_onTextureImageRetrieved = std::make_shared<EventSubscriber>([=](auto e) { onTextureImageRetrieved(e); });
    EventPublisher::subscribe(typeid(TextureImageRetrieved), m_onTextureImageRetrieved);
    m_onRetrieveTextureImageFailed = std::make_shared<EventSubscriber>([=](auto e) { onRetrieveTextureImageFailed(e); });
    EventPublisher::subscribe(typeid(RetrieveTextureImageFailed), m_onRetrieveTextureImageFailed);

    return ServiceResult::Complete;
}

ServiceResult TerrainEditService::onTerm()
{
    CommandBus::unsubscribe(typeid(CreateNewTerrain), m_createNewTerrain);
    m_createNewTerrain = nullptr;
    CommandBus::unsubscribe(typeid(MoveUpTerrainVertex), m_moveUpTerrainVertex);
    m_moveUpTerrainVertex = nullptr;
    CommandBus::unsubscribe(typeid(PaintTerrainTextureLayer), m_paintTerrainLayer);
    m_paintTerrainLayer = nullptr;
    CommandBus::unsubscribe(typeid(SaveTerrainSplatTexture), m_saveSplatTexture);
    m_saveSplatTexture = nullptr;
    CommandBus::unsubscribe(typeid(SavePickedTerrain), m_savePickedTerrain);
    m_savePickedTerrain = nullptr;

    //EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    //m_onSceneGraphBuilt = nullptr;
    //EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onTerrainPrimitiveBuilt);
    //m_onTerrainPrimitiveBuilt = nullptr;
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

void TerrainEditService::moveUpTerrainVertexByBrush(const Vector3& brush_pos, float brush_size, float height)
{
    assert((height > Math::Epsilon()) || (height < -Math::Epsilon()));
    if (m_pickedTerrain.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->getPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->getGeometryData());
    if (!terrain_geo) return;

    m_isHeightMapDirty = true;
    auto cell_dimension = terrain_geo->getCellDimension();
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
            Vector3 vtx_pos = Vector3(brush_pos.x() + x, brush_pos.y(), brush_pos.z() + z);
            moveUpTerrainVertex(terrain_geo, vtx_pos, y);
        }
    }
    commitHeightMapUpdated(terrain_prim, terrain_geo);
}

void TerrainEditService::moveUpTerrainVertex(const std::shared_ptr<TerrainGeometry>& terrain_geometry, const Vector3& picking_pos, float height)
{
    if (m_pickedTerrain.expired()) return;
    if (terrain_geometry == nullptr) return;

    Matrix4 world_inv = m_pickedTerrain.lock()->getWorldTransform().Inverse();
    Vector3 local_pick_pos = world_inv.TransformCoord(picking_pos);

    auto [cell_x, cell_z] = terrain_geometry->locateCell(local_pick_pos);

    terrain_geometry->changeHeight(cell_x, cell_z, terrain_geometry->getHeight(cell_x, cell_z) + height);
    auto vtx_idx = terrain_geometry->convertVertexIndex(cell_x, cell_z);
    if (m_dirtyVtxMinIndex > vtx_idx) m_dirtyVtxMinIndex = vtx_idx;
    if (m_dirtyVtxMaxIndex < vtx_idx) m_dirtyVtxMaxIndex = vtx_idx;
}

void TerrainEditService::commitHeightMapUpdated(const std::shared_ptr<TerrainPrimitive>& terrain_primitive,
    const std::shared_ptr<TerrainGeometry>& terrain_geometry)
{
    if (terrain_primitive == nullptr) return;
    if (terrain_geometry == nullptr) return;

    if (m_dirtyVtxMaxIndex < m_dirtyVtxMinIndex) return;

    terrain_geometry->rangedUpdateHeightMapToVertexMemory(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1);
    terrain_geometry->rangedUpdateVertexNormals(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1);
    terrain_primitive->rangedUpdateRenderBuffer(m_dirtyVtxMinIndex, m_dirtyVtxMaxIndex - m_dirtyVtxMinIndex + 1, std::nullopt, std::nullopt);
    m_dirtyVtxMaxIndex = std::numeric_limits<unsigned int>::min();
    m_dirtyVtxMinIndex = std::numeric_limits<unsigned int>::max();
}

void TerrainEditService::paintTerrainLayerByBrush(const Vector3& brush_pos, float brush_size, unsigned layer_idx, float density)
{
    assert(layer_idx < TextureLayerNum);
    if (m_pickedTerrain.expired()) return;
    if (m_pickedSplatTexture.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->getPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->getGeometryData());
    if (!terrain_geo) return;

    auto cell_dimension = terrain_geo->getCellDimension();
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
            Vector3 vtx_pos = Vector3(brush_pos.x() + x, brush_pos.y(), brush_pos.z() + z);
            paintTerrainLayer(vtx_pos, layer_idx, y);
        }
    }
    commitAlphaTexelUpdated();
}

void TerrainEditService::paintTerrainLayer(const Vector3& picking_pos, unsigned layer_idx, float density)
{
    assert(layer_idx < TextureLayerNum);
    if (m_pickedTerrain.expired()) return;
    if (m_pickedSplatTexture.expired()) return;
    const auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(m_pickedTerrain.lock()->getPrimitive());
    if (!terrain_prim) return;
    auto terrain_geo = std::dynamic_pointer_cast<TerrainGeometry>(terrain_prim->getGeometryData());
    if (!terrain_geo) return;

    Matrix4 world_inv = m_pickedTerrain.lock()->getWorldTransform().Inverse();
    Vector3 local_pick_pos = world_inv.TransformCoord(picking_pos);
    auto min_local_pos = terrain_geo->getMinPosition();

    auto num_cols = terrain_geo->getNumCols();
    auto num_rows = terrain_geo->getNumRows();
    auto cell_dimension = terrain_geo->getCellDimension();
    auto tex_dimension = m_pickedSplatTexture.lock()->dimension();
    auto texel_per_cell_x = tex_dimension.m_width / num_cols;
    auto texel_per_cell_z = tex_dimension.m_height / num_rows;
    float texel_grid_size_x = cell_dimension.m_width / static_cast<float>(texel_per_cell_x);
    float texel_grid_size_z = cell_dimension.m_height / static_cast<float>(texel_per_cell_z);
    auto texel_x = static_cast<unsigned>((local_pick_pos.x() - min_local_pos.x()) / texel_grid_size_x);
    if (texel_x >= tex_dimension.m_width) return;
    auto texel_z = static_cast<unsigned>((local_pick_pos.z() - min_local_pos.z()) / texel_grid_size_z);
    if (texel_z >= tex_dimension.m_height) return;
    texel_z = (tex_dimension.m_height - 1) - texel_z;  // invert texel z, bcz texcoord is y-down
    if (m_dirtyAlphaRect.Left() > static_cast<int>(texel_x)) m_dirtyAlphaRect.Left() = static_cast<int>(texel_x);
    if (m_dirtyAlphaRect.Right() < static_cast<int>(texel_x + 1)) m_dirtyAlphaRect.Right() = static_cast<int>(texel_x + 1);
    if (m_dirtyAlphaRect.Top() > static_cast<int>(texel_z)) m_dirtyAlphaRect.Top() = static_cast<int>(texel_z);
    if (m_dirtyAlphaRect.Bottom() < static_cast<int>(texel_z + 1)) m_dirtyAlphaRect.Bottom() = static_cast<int>(texel_z + 1);
    addLayerAlpha(texel_x, texel_z, layer_idx, static_cast<int>(density * 255));
}

void TerrainEditService::addLayerAlpha(unsigned texel_x, unsigned texel_y, unsigned layer_idx, int density)
{
    if (m_alphaTexels.empty()) return;
    if (m_pickedSplatTexture.expired()) return;

    auto dimension = m_pickedSplatTexture.lock()->dimension();
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

void TerrainEditService::commitAlphaTexelUpdated()
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
    CommandBus::enqueue(std::make_shared<EnqueueUpdatingTextureImage>(m_pickedSplatTexture.lock(), m_dirtyAlphaRect, m_dirtyAlphaTexels));
    m_dirtyAlphaRect.Left() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Right() = std::numeric_limits<int>::min();
    m_dirtyAlphaRect.Top() = std::numeric_limits<int>::max();
    m_dirtyAlphaRect.Bottom() = std::numeric_limits<int>::min();
}

void TerrainEditService::assembleTerrainGeometry(const std::shared_ptr<CreateNewTerrain>& cmd)
{
    TerrainGeometryAssembler geometry_assembler(cmd->geometryId());
    geometry_assembler.numRows(cmd->numRows()).numCols(cmd->numCols()).minPosition(cmd->minVtxPos()).maxPosition(cmd->maxVtxPos())
        .minTextureCoordinate(cmd->minUV()).maxTextureCoordinate(cmd->maxUV()).asAsset(cmd->geometryId().name(), cmd->geometryId().name() + ".geo", cmd->assetPathId());
    auto terrain_geo = std::make_shared<Enigma::Geometries::RequestGeometryConstitution>(cmd->geometryId(), geometry_assembler.dto().toGenericDto())->dispatch();
    assert(terrain_geo);
    std::make_shared<Enigma::Geometries::PutGeometry>(cmd->geometryId(), terrain_geo)->execute();
}

std::tuple<TextureId, std::shared_ptr<Texture>> TerrainEditService::assembleTerrainSplatTexture(const std::shared_ptr<CreateNewTerrain>& cmd)
{
    TextureDto dto;
    auto splatTextureId = TextureId(cmd->geometryId().name() + "_splat");
    auto asset_filename = splatTextureId.name() + ".tex@" + cmd->assetPathId();
    auto image_filename = splatTextureId.name() + ".png@" + cmd->assetPathId();
    dto.id(splatTextureId);
    dto.factoryDesc(FactoryDesc(Texture::TYPE_RTTI.getName()).ClaimAsResourceAsset(splatTextureId.name(), asset_filename));
    dto.format(Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8);
    dto.dimension(Enigma::MathLib::Dimension<unsigned>{ 512, 512 });
    dto.dimensionOfCreation(Enigma::MathLib::Dimension<unsigned>{ 512, 512 });
    dto.isCubeTexture(false);
    dto.surfaceCount(1);
    dto.filePaths({ image_filename });
    std::shared_ptr<Texture> splatTexture = std::make_shared<RequestTextureConstitution>(splatTextureId, dto.toGenericDto())->dispatch();
    std::make_shared<PutTexture>(splatTextureId, splatTexture)->enqueue();
    //if (!m_textureFileStoreMapper.expired()) m_textureFileStoreMapper.lock()->putTexture(splatTextureId, dto.toGenericDto());
    return { splatTextureId, splatTexture };
}

Enigma::Primitives::PrimitiveId TerrainEditService::assembleTerrainPrimitive(const std::shared_ptr<CreateNewTerrain>& cmd, const TextureId& splat_texture_id)
{
    auto terrain_prim_id = Enigma::Primitives::PrimitiveId(cmd->geometryId().name(), TerrainPrimitive::TYPE_RTTI);
    TerrainPrimitiveAssembler primitive_assembler(terrain_prim_id);
    primitive_assembler.meshPrimitive().geometryId(cmd->geometryId()).visualTechnique("Default").effect(EffectMaterialId("fx/TerrainMesh"));
    for (unsigned i = 0; i < TextureLayerNum; i++)
    {
        if (cmd->layerTextures()[i].name().empty()) continue;
        primitive_assembler.meshPrimitive().textureMap(EffectTextureMapAssembler().textureMapping(cmd->layerTextures()[i], std::nullopt, LayerSemantics[i]));
    }
    primitive_assembler.meshPrimitive().textureMap(EffectTextureMapAssembler().textureMapping(splat_texture_id, std::nullopt, ALPHA_TEXTURE_SEMANTIC));
    primitive_assembler.asNative(terrain_prim_id.name() + ".mesh@" + cmd->assetPathId());
    auto terrain_primitive = std::make_shared<Enigma::Primitives::RequestPrimitiveConstitution>(terrain_prim_id, primitive_assembler.toGenericDto())->dispatch();
    assert(terrain_primitive);
    std::make_shared<Enigma::Primitives::PutPrimitive>(terrain_prim_id, terrain_primitive)->execute();
    return terrain_prim_id;
}

std::shared_ptr<TerrainPawn> TerrainEditService::assembleTerrainPawn(const std::shared_ptr<CreateNewTerrain>& cmd, const Enigma::Primitives::PrimitiveId& terrain_primitive_id)
{
    auto pawn_id = SpatialId(cmd->geometryId().name(), TerrainPawn::TYPE_RTTI);
    TerrainPawnAssembler pawn_assembler(pawn_id);
    pawn_assembler.pawn().primitive(terrain_primitive_id);
    pawn_assembler.pawn().spatial().localTransform(Matrix4::MakeTranslateTransform(cmd->localPosistion()));
    pawn_assembler.asNative(pawn_id.name() + ".terrain@" + cmd->assetPathId());
    auto terrain = std::make_shared<RequestSpatialConstitution>(pawn_id, pawn_assembler.toGenericDto())->dispatch();
    assert(terrain);
    std::make_shared<PutSpatial>(pawn_id, terrain)->execute();
    return std::dynamic_pointer_cast<TerrainPawn>(terrain);
}

void TerrainEditService::createNewTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateNewTerrain, ICommand>(c);
    if (!cmd) return;
    m_terrainPathId = cmd->assetPathId();

    assembleTerrainGeometry(cmd);
    auto [splatTextureId, splatTexture] = assembleTerrainSplatTexture(cmd);
    auto terrain_prim_id = assembleTerrainPrimitive(cmd, splatTextureId);
    auto terrain = assembleTerrainPawn(cmd, terrain_prim_id);
    CommandBus::enqueue(std::make_shared<AttachNodeChild>(cmd->ownerNodeId(), terrain, terrain->getLocalTransform()));
    m_pickedTerrain = terrain;
    m_pickedSplatTexture = splatTexture;
    m_splatTextures.insert_or_assign(terrain->id(), splatTexture);
}

void TerrainEditService::moveUpTerrainVertex(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<MoveUpTerrainVertex, ICommand>(c);
    if (!cmd) return;
    moveUpTerrainVertexByBrush(cmd->getBrushPos(), cmd->getBrushSize(), cmd->getBrushHeight());
}

void TerrainEditService::paintTerrainLayer(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PaintTerrainTextureLayer, ICommand>(c);
    if (!cmd) return;
    paintTerrainLayerByBrush(cmd->getBrushPos(), cmd->getBrushSize(), cmd->getTextureLayerIndex(), cmd->getBrushDensity());
}

void TerrainEditService::saveSplatTexture(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<SaveTerrainSplatTexture, ICommand>(c);
    if (!cmd) return;
    if (m_savingSplatTextureFile)
    {
        CommandBus::enqueue(std::make_shared<OutputMessage>("Save Splat Texture : previous saving not complete yet"));
        return;
    }
    if ((m_pickedSplatTexture.expired()) || (m_pickedSplatTexture.lock()->filePaths().empty()))
    {
        CommandBus::enqueue(std::make_shared<OutputMessage>("Save Splat Texture : No splat texture selected or no image file assigned"));
        return;
    }
    if (m_pickedTerrain.expired())
    {
        CommandBus::enqueue(std::make_shared<OutputMessage>("Save Splat Texture : No terrain selected"));
        return;
    }
    CommandBus::enqueue(std::make_shared<PutTexture>(m_pickedSplatTexture.lock()->id(), m_pickedSplatTexture.lock()));
    m_savingSplatTextureFile = FileSystem::instance()->openFile(m_pickedSplatTexture.lock()->filePaths()[0], write | openAlways | binary);
    CommandBus::enqueue(std::make_shared<EnqueueSavingTexture>(m_pickedSplatTexture.lock(), m_savingSplatTextureFile));
}

void TerrainEditService::savePickedTerrain(const ICommandPtr& c)
{
    if (m_pickedTerrain.expired()) return;
    std::make_shared<PutSpatial>(m_pickedTerrain.lock()->id(), m_pickedTerrain.lock())->enqueue();
}

/*void TerrainEditService::onSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != NEW_TERRAIN_TAG) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!terrain) return;
    terrain->factoryDesc().ClaimAsInstanced(terrain->id().name() + ".pawn", m_terrainPathId);
    CommandBus::enqueue(std::make_shared<AttachTerrainToWorldMap>(terrain, terrain->getLocalTransform()));
}

void TerrainEditService::onTerrainPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn>(ev->pawn());
    if (!terrain) return;
    auto terrain_prim = std::dynamic_pointer_cast<TerrainPrimitive>(terrain->getPrimitive());
    if (!terrain_prim) return;
    terrain_prim->factoryDesc().ClaimAsInstanced(terrain_prim->getName() + ".terrain", m_terrainPathId);
    auto splat_texture = terrain_prim->findTextureBySemantic(ALPHA_TEXTURE_SEMANTIC);
    if (!splat_texture) return;
    std::string splat_texture_resouce_name = splat_texture->id().name();
    splat_texture->factoryDesc().ClaimAsResourceAsset(splat_texture_resouce_name, splat_texture_resouce_name + ".png", m_terrainPathId);
    m_splatTextures.insert_or_assign(terrain->id(), splat_texture);
}*/

void TerrainEditService::onPickedSpatialChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PickedSpatialChanged, IEvent>(e);
    if (!ev) return;
    m_pickedTerrain.reset();
    m_pickedSplatTexture.reset();
    auto picked_spatial = std::make_shared<QueryRunningSpatial>(ev->id())->dispatch();
    if (!picked_spatial) return;
    if (const auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(picked_spatial)) m_pickedTerrain = terrain;
    if (m_pickedTerrain.expired()) return;
    if (const auto splat_texture = m_splatTextures.find(m_pickedTerrain.lock()->id()); splat_texture != m_splatTextures.end())
    {
        m_pickedSplatTexture = splat_texture->second;
    }
    if (m_pickedSplatTexture.expired()) return;
    m_alphaRect = { 0, 0, static_cast<int>(m_pickedSplatTexture.lock()->dimension().m_width), static_cast<int>(m_pickedSplatTexture.lock()->dimension().m_height) };
    CommandBus::enqueue(std::make_shared<EnqueueRetrievingTextureImage>(m_pickedSplatTexture.lock(), m_alphaRect));
}

void TerrainEditService::onTextureImageRetrieved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureImageRetrieved, IEvent>(e);
    if (!ev) return;
    if (m_pickedSplatTexture.expired()) return;
    if (ev->id() != m_pickedSplatTexture.lock()->id()) return;
    m_alphaTexels = ev->imageBuffer();
}

void TerrainEditService::onRetrieveTextureImageFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RetrieveTextureImageFailed, IEvent>(e);
    if (!ev) return;
    if (ev->error())
    {
        CommandBus::enqueue(std::make_shared<OutputMessage>(string_format("Retrieve splat texture %s image failed %s", ev->id().name().c_str(), ev->error().message().c_str())));
    }
}

void TerrainEditService::onSplatTextureSaved(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureSaved, IEvent>(e);
    if (!ev) return;
    if (!m_savingSplatTextureFile) return;
    if (m_pickedSplatTexture.expired())
    {
        FileSystem::instance()->closeFile(m_savingSplatTextureFile);
        m_savingSplatTextureFile = nullptr;
        return;
    }
    if (ev->id() != m_pickedSplatTexture.lock()->id()) return;
    FileSystem::instance()->closeFile(m_savingSplatTextureFile);
    m_savingSplatTextureFile = nullptr;
}

void TerrainEditService::onSaveSplatTextureFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SaveTextureFailed, IEvent>(e);
    if (!ev) return;
    if (!m_savingSplatTextureFile) return;
    if (m_pickedSplatTexture.expired())
    {
        FileSystem::instance()->closeFile(m_savingSplatTextureFile);
        m_savingSplatTextureFile = nullptr;
        return;
    }
    if (ev->id() != m_pickedSplatTexture.lock()->id()) return;
    FileSystem::instance()->closeFile(m_savingSplatTextureFile);
    m_savingSplatTextureFile = nullptr;
    CommandBus::enqueue(std::make_shared<OutputMessage>(string_format("Save splat texture %s image failed %s", ev->id().name().c_str(), ev->error().message().c_str())));
}
