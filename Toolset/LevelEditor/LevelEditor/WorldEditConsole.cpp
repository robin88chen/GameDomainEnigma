#include "WorldEditConsole.h"
#include "WorldMapFileStoreMapper.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditConsole::TYPE_RTTI{ "LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI };

WorldEditConsole::WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::string& world_relate_path, const std::string& media_path_id) : ISystemService(srv_mngr), m_worldRelativePath(world_relate_path), m_mediaPathId(media_path_id)
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

