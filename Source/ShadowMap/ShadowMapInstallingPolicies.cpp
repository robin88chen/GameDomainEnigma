#include "ShadowMapInstallingPolicies.h"
#include "CascadeShadowMapService.h"
#include "GameCommon/GameSceneService.h"
#include "Rendering/SceneRendererService.h"
#include "GameCommon/GameCameraService.h"
#include "Renderer/RendererManager.h"
#include "ShadowMapService.h"
#include "ShadowMapServiceConfiguration.h"

using namespace Enigma::ShadowMap;

error ShadowMapInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->getSystemServiceAs<GameCommon::GameSceneService>();
    auto camera_service = service_manager->getSystemServiceAs<GameCommon::GameCameraService>();
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto shadowmap_service = std::make_shared<ShadowMapService>(service_manager, scene_service, camera_service, render_manager, m_config);
    service_manager->registerSystemService(shadowmap_service);
    shadowmap_service->createShadowRenderSystem(m_rendererName, m_targetName);
    return error();
}

error ShadowMapInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(ShadowMapService::TYPE_RTTI);
    return error();
}

error CascadeShadowMapInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->getSystemServiceAs<GameCommon::GameSceneService>();
    auto camera_service = service_manager->getSystemServiceAs<GameCommon::GameCameraService>();
    auto render_manager = service_manager->getSystemServiceAs<Renderer::RendererManager>();
    auto shadowmap_service = std::make_shared<CascadeShadowMapService>(service_manager, scene_service, camera_service, render_manager, m_config);
    service_manager->registerSystemService(shadowmap_service);
    service_manager->insertHashAsService(ShadowMapService::TYPE_RTTI, shadowmap_service);
    shadowmap_service->createShadowRenderSystem(m_rendererName, m_targetName);
    return error();
}

error CascadeShadowMapInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->removeHashAsService(ShadowMapService::TYPE_RTTI);
    service_manager->shutdownSystemService(CascadeShadowMapService::TYPE_RTTI);
    return error();
}
