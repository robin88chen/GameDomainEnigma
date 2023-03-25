#include "SceneGraphInstallingPolicy.h"
#include "GameEngine/TimerService.h"
#include "SceneGraphRepository.h"
#include "LazyNodeIOService.h"
#include "SceneGraphErrors.h"
#include <cassert>

using namespace Enigma::SceneGraph;

error SceneGraphInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    const auto timer = service_manager->GetSystemServiceAs<Engine::TimerService>();
    assert(timer);
    service_manager->RegisterSystemService(std::make_shared<SceneGraphRepository>(service_manager, m_dtoDeserializer, m_effectDeserializer));
    service_manager->RegisterSystemService(std::make_shared<LazyNodeIOService>(service_manager, timer, m_dtoDeserializer));
    return ErrorCode::ok;
}

error SceneGraphInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(SceneGraph::LazyNodeIOService::TYPE_RTTI);
    service_manager->ShutdownSystemService(SceneGraph::SceneGraphRepository::TYPE_RTTI);
    return ErrorCode::ok;
}
