#include "SceneRendererInstallingPolicy.h"
#include "SceneRendering.h"
#include "DeferredRendering.h"
#include "Renderer/RendererManager.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "LightVolumePawn.h"
#include "LightQuadPawn.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::Rendering;

error SceneRendererInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto scene_renderer_service = std::make_shared<SceneRendering>(service_manager, render_manager, m_config);
    service_manager->registerSystemService(scene_renderer_service);
    scene_renderer_service->createSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error SceneRendererInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(SceneRendering::TYPE_RTTI);
    return error();
}

error DeferredRendererInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto deferred_renderer_service = std::make_shared<DeferredRendering>(service_manager,
        render_manager, m_config);
    service_manager->registerSystemService(deferred_renderer_service);
    service_manager->insertHashAsService(SceneRendering::TYPE_RTTI, deferred_renderer_service);

    Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::RegisterSpatialFactory>(LightVolumePawn::TYPE_RTTI.getName(), LightVolumePawn::create, LightVolumePawn::constitute));
    Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::RegisterSpatialFactory>(LightQuadPawn::TYPE_RTTI.getName(), LightQuadPawn::create, LightQuadPawn::constitute));
    deferred_renderer_service->createSceneRenderSystem(m_rendererName, m_targetName);
    return error();
}

error DeferredRendererInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    //Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(LightVolumePawn::TYPE_RTTI.name()));
    //Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(LightQuadPawn::TYPE_RTTI.name()));
    service_manager->removeHashAsService(SceneRendering::TYPE_RTTI);
    service_manager->shutdownSystemService(DeferredRendering::TYPE_RTTI);
    return error();
}
