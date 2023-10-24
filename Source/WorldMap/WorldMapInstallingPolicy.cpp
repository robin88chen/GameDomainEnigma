#include "WorldMapInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "SceneGraph/SceneGraphErrors.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "WorldMap.h"
#include "WorldMapService.h"
#include <cassert>

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;

error WorldMapInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_graph = service_manager->GetSystemServiceAs<SceneGraphRepository>();
    Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(WorldMap::TYPE_RTTI.getName(),
        [](auto o) { return new WorldMap(o); }));
    service_manager->RegisterSystemService(std::make_shared<WorldMapService>(service_manager, scene_graph));
    return ErrorCode::ok;
}

error WorldMapInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(WorldMap::TYPE_RTTI.getName()));
    service_manager->ShutdownSystemService(WorldMapService::TYPE_RTTI);
    return ErrorCode::ok;
}