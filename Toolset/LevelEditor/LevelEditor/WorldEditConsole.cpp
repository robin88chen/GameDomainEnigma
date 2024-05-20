#include "WorldEditConsole.h"
#include "WorldMapFileStoreMapper.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditConsole::TYPE_RTTI{ "LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI };

WorldEditConsole::WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldMapFileStoreMapper>& world_map_file_store_mapper, const std::string& world_relate_path, const std::string& media_path_id) : ISystemService(srv_mngr), m_worldMapFileStoreMapper(world_map_file_store_mapper), m_worldRelativePath(world_relate_path), m_mediaPathId(media_path_id)
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

bool WorldEditConsole::isWorldNameDuplicated(const std::string& world_name) const
{
    auto world_path_name = m_worldRelativePath + "/" + world_name;
    if (auto world_map_file_store_mapper = m_worldMapFileStoreMapper.lock())
    {
        return world_map_file_store_mapper->isWorldNameDuplicated(world_path_name);
    }
    return false;
}
