#include "TerrainEditService.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti TerrainEditService::TYPE_RTTI{"LevelEditor.TerrainEditService", & ISystemService::TYPE_RTTI};

TerrainEditService::TerrainEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    
}

TerrainEditService::~TerrainEditService()
{
    
}

ServiceResult TerrainEditService::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult TerrainEditService::OnTerm()
{
    return ServiceResult::Complete;
}

