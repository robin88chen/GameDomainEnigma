#include "WorldEditService.h"

#include <Frameworks/CommandBus.h>
#include <SceneGraph/SceneGraphCommands.h>

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditService::TYPE_RTTI{"LevelEditor.WorldEditService", &ISystemService::TYPE_RTTI};

std::string LoadingWorldTag = "_LoadingWorld_";

WorldEditService::WorldEditService(ServiceManager* srv_mngr, const std::shared_ptr<Enigma::WorldMap::WorldMapService>& map) : ISystemService(srv_mngr)
{
    m_worldMap = map;
    m_needTick = false;
}

WorldEditService::~WorldEditService()
{
    
}

ServiceResult WorldEditService::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult WorldEditService::OnTerm()
{
    return ServiceResult::Complete;
}

std::tuple<Enigma::Engine::GenericDtoCollection, std::vector<Enigma::Engine::GenericDtoCollection>> WorldEditService::SerializeWorldMapAndNodeGraphs(const std::string& path_id) const
{
    assert(!m_worldMap.expired());
    auto node_collections = m_worldMap.lock()->SerializeQuadNodeGraphs();
    for (auto& dtos : node_collections)
    {
        auto desc = dtos[0].GetRtti();
        desc.PathId(path_id);
        dtos[0].AddRtti(desc);
    }
    auto map_graph = m_worldMap.lock()->SerializeWorldMap();
    if (!map_graph.empty())
    {
        for (auto& d : map_graph)
        {
            auto desc = d.GetRtti();
            desc.PathId(path_id);
            d.AddRtti(desc);
        }
    }
    return { map_graph, node_collections };
}

void WorldEditService::DeserializeWorldMap(const Enigma::Engine::GenericDtoCollection& world_map_dto)
{
    Enigma::Frameworks::CommandBus::Post(std::make_shared<Enigma::SceneGraph::BuildSceneGraph>(LoadingWorldTag, world_map_dto));
}
