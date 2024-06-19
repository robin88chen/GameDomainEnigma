#include "WorldEditConsole.h"
#include "WorldMapFileStoreMapper.h"
#include "WorldEditService.h"
#include "WorldMap/WorldMapCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditConsole::TYPE_RTTI{ "LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI };

WorldEditConsole::WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldEditService>& world_edit_service, const std::string& world_relate_path, const std::string& media_path_id) : ISystemService(srv_mngr), m_worldEditService(world_edit_service), m_worldRelativePath(world_relate_path), m_mediaPathId(media_path_id)
{
    m_needTick = false;
}

WorldEditConsole::~WorldEditConsole()
{
}

Enigma::Frameworks::ServiceResult WorldEditConsole::onInit()
{
    return ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult WorldEditConsole::onTerm()
{
    return ServiceResult::Complete;
}

void WorldEditConsole::saveWorldMap()
{
    auto world_edit_service = m_worldEditService.lock();
    if (!world_edit_service) return;

    auto world_map = world_edit_service->getWorldMap();
    if (!world_map) return;
    std::make_shared<Enigma::WorldMap::PutWorldMap>(world_map->id(), world_map)->execute();
}

