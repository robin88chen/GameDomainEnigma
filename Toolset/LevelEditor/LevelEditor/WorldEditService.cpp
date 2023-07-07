#include "WorldEditService.h"
#include <filesystem>

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditService::TYPE_RTTI{"LevelEditor.WorldEditService", &ISystemService::TYPE_RTTI};

WorldEditService::WorldEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
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

void WorldEditService::SetWorldMapRootFolder(const std::filesystem::path& folder)
{
    m_mapFileRootPath = folder;
}

