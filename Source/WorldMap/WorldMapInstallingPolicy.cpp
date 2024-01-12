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

error WorldMapInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_graph = service_manager->getSystemServiceAs<SceneGraphRepository>();
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(WorldMap::TYPE_RTTI.getName(),
      //  [scene_graph](const Engine::GenericDto& o) { return new WorldMap(scene_graph, o); }));
    service_manager->registerSystemService(std::make_shared<WorldMapService>(service_manager, scene_graph));
    return ErrorCode::ok;
}

error WorldMapInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(WorldMap::TYPE_RTTI.getName()));
    service_manager->shutdownSystemService(WorldMapService::TYPE_RTTI);
    return ErrorCode::ok;
}