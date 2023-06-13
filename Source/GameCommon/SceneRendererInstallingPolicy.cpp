#include "SceneRendererInstallingPolicy.h"
#include "SceneRendererService.h"
#include "GameCameraService.h"
#include "GameSceneService.h"
#include "DeferredRendererService.h"
#include "Renderer/RendererManager.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "LightVolumePawn.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::GameCommon;

error SceneRendererInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->GetSystemServiceAs<GameSceneService>();
    auto camera_service = service_manager->GetSystemServiceAs<GameCameraService>();
    auto render_manager = service_manager->GetSystemServiceAs<Renderer::RendererManager>();
    auto scene_renderer_service = std::make_shared<SceneRendererService>(service_manager,
        scene_service, camera_service, render_manager, std::move(m_config));
    service_manager->RegisterSystemService(scene_renderer_service);
    scene_renderer_service->CreateSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error SceneRendererInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(SceneRendererService::TYPE_RTTI);
    return error();
}

error DeferredRendererInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->GetSystemServiceAs<GameSceneService>();
    auto camera_service = service_manager->GetSystemServiceAs<GameCameraService>();
    auto render_manager = service_manager->GetSystemServiceAs<Renderer::RendererManager>();
    auto scene_graph_repo = service_manager->GetSystemServiceAs<SceneGraph::SceneGraphRepository>();
    auto deferred_renderer_service = std::make_shared<DeferredRendererService>(service_manager,
               scene_service, camera_service, render_manager, scene_graph_repo, std::move(m_config));
    service_manager->RegisterSystemService(deferred_renderer_service);
    service_manager->InsertHashAsService(SceneRendererService::TYPE_RTTI, deferred_renderer_service);

    Frameworks::CommandBus::Post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(LightVolumePawn::TYPE_RTTI.GetName(),
        [=](auto dto) { return new LightVolumePawn(dto); }));
    deferred_renderer_service->CreateSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error DeferredRendererInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::Post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(LightVolumePawn::TYPE_RTTI.GetName()));
    service_manager->RemoveHashAsService(SceneRendererService::TYPE_RTTI);
    service_manager->ShutdownSystemService(DeferredRendererService::TYPE_RTTI);
    return error();
}
