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
#include "WorldMap/WorldMapCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::Terrain;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::WorldMap;

Rtti TerrainEditService::TYPE_RTTI{"LevelEditor.TerrainEditService", &ISystemService::TYPE_RTTI};

std::array<std::string, TerrainEditService::TextureLayerNum> layerSemantics =
{
    "TextureLayer0", "TextureLayer1", "TextureLayer2", "TextureLayer3"
};

const std::string NEW_TERRAIN_TAG = "_new_terrain_";

TerrainEditService::TerrainEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{

}

TerrainEditService::~TerrainEditService()
{

}

ServiceResult TerrainEditService::OnInit()
{
    m_doCreatingNewTerrain = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingNewTerrain(c); });
    CommandBus::Subscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    return ServiceResult::Complete;
}

ServiceResult TerrainEditService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(CreateNewTerrain), m_doCreatingNewTerrain);
    m_doCreatingNewTerrain = nullptr;
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;

    return ServiceResult::Complete;
}

void TerrainEditService::DoCreatingNewTerrain(const Enigma::Frameworks::ICommandPtr& c)
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
    for (unsigned i = 0; i < TerrainEditService::TextureLayerNum; i++)
    {
        if (cmd->GetLayerTextures()[i].empty()) continue;
        tex_dto.TextureMapping(cmd->GetLayerTextures()[i], "APK_PATH", cmd->GetLayerTextures()[i], std::nullopt, layerSemantics[i]);
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

void TerrainEditService::OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != NEW_TERRAIN_TAG) return;
    auto terrain = std::dynamic_pointer_cast<TerrainPawn, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!terrain) return;
    CommandBus::Post(std::make_shared<AttachTerrainToWorldMap>(terrain, terrain->GetLocalTransform()));
}
