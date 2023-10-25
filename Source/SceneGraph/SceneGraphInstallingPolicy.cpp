#include "SceneGraphInstallingPolicy.h"
#include "GameEngine/TimerService.h"
#include "SceneGraphRepository.h"
#include "LazyNodeIOService.h"
#include "LightInfoTraversal.h"
#include "SceneGraphErrors.h"
#include <cassert>

using namespace Enigma::SceneGraph;

error SceneGraphInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    const auto timer = service_manager->getSystemServiceAs<Engine::TimerService>();
    assert(timer);
    service_manager->registerSystemService(std::make_shared<SceneGraphRepository>(service_manager, m_dtoDeserializer));
    service_manager->registerSystemService(std::make_shared<LazyNodeIOService>(service_manager, timer, m_dtoDeserializer));
    service_manager->registerSystemService(std::make_shared<LightInfoTraversal>(service_manager));
    return ErrorCode::ok;
}

error SceneGraphInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(SceneGraph::LazyNodeIOService::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::SceneGraphRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::LightInfoTraversal::TYPE_RTTI);
    return ErrorCode::ok;
}
