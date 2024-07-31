#include "WorldEditService.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "WorldMap/WorldMapCommands.h"
#include "WorldMap/WorldMapEvents.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditService::TYPE_RTTI{ "LevelEditor.WorldEditService", &ISystemService::TYPE_RTTI };

WorldEditService::WorldEditService(ServiceManager* srv_mngr, const std::shared_ptr<Enigma::WorldMap::WorldMapRepository>& world_map_repository) : ISystemService(srv_mngr)
{
    m_worldMapRepository = world_map_repository;
    m_needTick = false;
    m_worldMap = nullptr;
}

WorldEditService::~WorldEditService()
{
    m_worldMap = nullptr;
}

ServiceResult WorldEditService::onInit()
{
    m_onWorldMapConstituted = std::make_shared<EventSubscriber>([=](const IEventPtr& e) { onWorldMapCreatedOrConstituted(e); });
    EventPublisher::subscribe(typeid(Enigma::WorldMap::WorldMapConstituted), m_onWorldMapConstituted);
    m_onWorldMapCreated = std::make_shared<EventSubscriber>([=](const IEventPtr& e) { onWorldMapCreatedOrConstituted(e); });
    EventPublisher::subscribe(typeid(Enigma::WorldMap::WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapRemoved = std::make_shared<EventSubscriber>([=](const IEventPtr& e) { onWorldMapRemoved(e); });
    EventPublisher::subscribe(typeid(Enigma::WorldMap::WorldMapRemoved), m_onWorldMapRemoved);

    return ServiceResult::Complete;
}

ServiceResult WorldEditService::onTerm()
{
    m_worldMap = nullptr;
    m_worldMapRepository.reset();
    EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapConstituted), m_onWorldMapConstituted);
    m_onWorldMapConstituted = nullptr;
    EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapCreated = nullptr;
    EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapRemoved), m_onWorldMapRemoved);
    m_onWorldMapRemoved = nullptr;

    return ServiceResult::Complete;
}

std::shared_ptr<Enigma::WorldMap::WorldMap> WorldEditService::getWorldMap() const
{
    return m_worldMap;
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
    Enigma::Frameworks::CommandBus::enqueue(std::make_shared<Enigma::WorldMap::DeserializeWorldMap>(world_map_dto[0].getName(), world_map_dto, portal_manager_name));
}

void WorldEditService::onWorldMapCreatedOrConstituted(const Enigma::Frameworks::IEventPtr& e)
{
    if (m_worldMapRepository.expired()) return;
    if (!e) return;
    if (auto ev = std::static_pointer_cast<Enigma::WorldMap::WorldMapConstituted>(e))
    {
        m_worldMap = ev->map();
    }
    else if (auto ev = std::static_pointer_cast<Enigma::WorldMap::WorldMapCreated>(e))
    {
        m_worldMap = ev->map();
    }
}

void WorldEditService::onWorldMapRemoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (m_worldMapRepository.expired()) return;
    if (!m_worldMap) return;
    if (!e) return;
    if (auto ev = std::static_pointer_cast<Enigma::WorldMap::WorldMapRemoved>(e))
    {
        if (m_worldMap->id() == ev->id())
        {
            m_worldMap = nullptr;
        }
    }
}