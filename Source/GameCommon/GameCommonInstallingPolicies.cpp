#include "GameCommonInstallingPolicies.h"
#include "Frameworks/ServiceManager.h"
#include "GameCameraService.h"
#include "GameSceneService.h"
#include "GameLightService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "AnimatedPawn.h"
#include <cassert>

using namespace Enigma::GameCommon;

error GameCameraInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto camera_service = std::make_shared<GameCameraService>(service_manager,
        service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>());
    service_manager->registerSystemService(camera_service);
    camera_service->constitutePrimaryCamera(m_cameraId, m_cameraDto);
    return error();
}

error GameCameraInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(GameCameraService::TYPE_RTTI);
    return error();
}

error GameLightInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto light_service = std::make_shared<GameLightService>(service_manager,
        service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>());
    service_manager->registerSystemService(light_service);
    return error();
}

error GameLightInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(GameLightService::TYPE_RTTI);
    return error();
}

error GameSceneInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    auto camera_service = service_manager->getSystemServiceAs<GameCameraService>();
    auto scene_service = std::make_shared<GameSceneService>(service_manager, scene_graph_repository, camera_service);
    service_manager->registerSystemService(scene_service);
    scene_service->createRootScene(m_sceneRootId, m_portalManagedName);
    return error();
}

error GameSceneInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(GameSceneService::TYPE_RTTI);
    return error();
}

error AnimatedPawnInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(AnimatedPawn::TYPE_RTTI.getName(),
      //  [](auto o) { return new AnimatedPawn(o); }));
    return error();
}

error AnimatedPawnInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(AnimatedPawn::TYPE_RTTI.getName()));
    return error();
}
