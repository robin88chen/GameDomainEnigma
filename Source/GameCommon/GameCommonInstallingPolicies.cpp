#include "GameCommonInstallingPolicies.h"
#include "Frameworks/ServiceManager.h"
#include "GameCameraService.h"
#include "GameSceneService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "AnimatedPawn.h"
#include "AvatarRecipeFactory.h"
#include "AvatarRecipes.h"
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

error GameSceneInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    auto camera_service = service_manager->getSystemServiceAs<GameCameraService>();
    auto scene_service = std::make_shared<GameSceneService>(service_manager, scene_graph_repository, camera_service);
    service_manager->registerSystemService(scene_service);
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
    assert(service_manager);
    const auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    assert(scene_graph_repository);
    scene_graph_repository->registerSpatialFactory(AnimatedPawn::TYPE_RTTI.getName(), AnimatedPawn::create);
    auto avatar_factory = std::make_shared<AvatarRecipeFactory>(service_manager);
    service_manager->registerSystemService(avatar_factory);
    avatar_factory->registerRecipeFactory(ReplaceAvatarMaterial::TYPE_RTTI.getName(), ReplaceAvatarMaterial::create);
    avatar_factory->registerRecipeFactory(ChangeAvatarTexture::TYPE_RTTI.getName(), ChangeAvatarTexture::create);
    return error();
}

error AnimatedPawnInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    if (const auto avatar_factory = service_manager->getSystemServiceAs<AvatarRecipeFactory>())
    {
        avatar_factory->unregisterRecipeFactory(ReplaceAvatarMaterial::TYPE_RTTI.getName());
        avatar_factory->unregisterRecipeFactory(ChangeAvatarTexture::TYPE_RTTI.getName());
    }
    return error();
}
