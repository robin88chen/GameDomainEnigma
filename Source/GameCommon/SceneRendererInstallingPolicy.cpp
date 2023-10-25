#include "SceneRendererInstallingPolicy.h"
#include "SceneRendererService.h"
#include "GameCameraService.h"
#include "GameSceneService.h"
#include "DeferredRendererService.h"
#include "Renderer/RendererManager.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "LightVolumePawn.h"
#include "LightQuadPawn.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::GameCommon;

error SceneRendererInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->getSystemServiceAs<GameSceneService>();
    auto camera_service = service_manager->getSystemServiceAs<GameCameraService>();
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto scene_renderer_service = std::make_shared<SceneRendererService>(service_manager,
        scene_service, camera_service, render_manager, m_config);
    service_manager->registerSystemService(scene_renderer_service);
    scene_renderer_service->CreateSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error SceneRendererInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(SceneRendererService::TYPE_RTTI);
    return error();
}

error DeferredRendererInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->getSystemServiceAs<GameSceneService>();
    auto camera_service = service_manager->getSystemServiceAs<GameCameraService>();
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto deferred_renderer_service = std::make_shared<DeferredRendererService>(service_manager,
               scene_service, camera_service, render_manager, m_config);
    service_manager->registerSystemService(deferred_renderer_service);
    service_manager->insertHashAsService(SceneRendererService::TYPE_RTTI, deferred_renderer_service);

    Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(LightVolumePawn::TYPE_RTTI.getName(),
        [=](auto dto) { return new LightVolumePawn(dto); }));
    Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(LightQuadPawn::TYPE_RTTI.getName(),
        [=](auto dto) { return new LightQuadPawn(dto); }));
    deferred_renderer_service->CreateSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error DeferredRendererInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(LightVolumePawn::TYPE_RTTI.getName()));
    Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(LightQuadPawn::TYPE_RTTI.getName()));
    service_manager->removeHashAsService(SceneRendererService::TYPE_RTTI);
    service_manager->shutdownSystemService(DeferredRendererService::TYPE_RTTI);
    return error();
}
