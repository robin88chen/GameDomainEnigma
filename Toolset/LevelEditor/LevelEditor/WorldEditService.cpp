#include "WorldEditService.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "WorldMap/WorldMapCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditService::TYPE_RTTI{ "LevelEditor.WorldEditService", &ISystemService::TYPE_RTTI };

WorldEditService::WorldEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    //m_worldMap = map;
    m_needTick = false;
}

WorldEditService::~WorldEditService()
{

}

ServiceResult WorldEditService::onInit()
{
    return ServiceResult::Complete;
}

ServiceResult WorldEditService::onTerm()
{
    return ServiceResult::Complete;
}

std::tuple<Enigma::Engine::GenericDto, std::vector<Enigma::Engine::GenericDtoCollection>> WorldEditService::serializeWorldMapAndNodeGraphs(const std::string& path_id) const
{
    //assert(!m_worldMap.expired());
    //auto node_collections = m_worldMap.lock()->serializeWorldSceneGraphs();
    /*for (auto& dtos : node_collections)
    {
        auto desc = dtos[0].GetRtti();
        desc.PathId(path_id);
        dtos[0].AddRtti(desc);
    }*/
    //auto map_dto = m_worldMap.lock()->serializeWorldMap();
    /*auto desc = map_dto.GetRtti();
    desc.PathId(path_id);
    map_dto.AddRtti(desc);*/
    return { {}, {} };
}

void WorldEditService::deserializeWorldMap(const Enigma::Engine::GenericDtoCollection& world_map_dto, const std::string& portal_manager_name)
{
    assert(!world_map_dto.empty());
    Enigma::Frameworks::CommandBus::post(std::make_shared<Enigma::WorldMap::DeserializeWorldMap>(world_map_dto[0].getName(), world_map_dto, portal_manager_name));
}
