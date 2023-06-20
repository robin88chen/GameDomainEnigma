#include "ShadowMapInstallingPolicies.h"
#include "CascadeShadowMapService.h"
#include "GameCommon/GameSceneService.h"
#include "GameCommon/SceneRendererService.h"
#include "GameCommon/GameCameraService.h"
#include "Renderer/RendererManager.h"
#include "ShadowMapService.h"
#include "ShadowMapServiceConfiguration.h"

using namespace Enigma::ShadowMap;

error ShadowMapInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->GetSystemServiceAs<GameCommon::GameSceneService>();
    auto camera_service = service_manager->GetSystemServiceAs<GameCommon::GameCameraService>();
    auto render_manager = service_manager->GetSystemServiceAs<Renderer::RendererManager>();
    auto shadowmap_service = std::make_shared<ShadowMapService>(service_manager, scene_service, camera_service, render_manager, m_config);
    service_manager->RegisterSystemService(shadowmap_service);
    shadowmap_service->CreateShadowRenderSystem(m_rendererName, m_targetName);
    return error();
}

error ShadowMapInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(ShadowMapService::TYPE_RTTI);
    return error();
}

error CascadeShadowMapInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto scene_service = service_manager->GetSystemServiceAs<GameCommon::GameSceneService>();
    auto camera_service = service_manager->GetSystemServiceAs<GameCommon::GameCameraService>();
    auto render_manager = service_manager->GetSystemServiceAs<Renderer::RendererManager>();
    auto shadowmap_service = std::make_shared<CascadeShadowMapService>(service_manager, scene_service, camera_service, render_manager, m_config);
    service_manager->RegisterSystemService(shadowmap_service);
    service_manager->InsertHashAsService(ShadowMapService::TYPE_RTTI, shadowmap_service);
    shadowmap_service->CreateShadowRenderSystem(m_rendererName, m_targetName);
    return error();
}

error CascadeShadowMapInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->RemoveHashAsService(ShadowMapService::TYPE_RTTI);
    service_manager->ShutdownSystemService(CascadeShadowMapService::TYPE_RTTI);
    return error();
}
