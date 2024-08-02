#include "DeferredRendererService.h"
#include "DeferredRendererServiceConfiguration.h"
#include "LightVolumePawn.h"
#include "Controllers/GraphicMain.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/DeferredRenderer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "Geometries/GeometryDataQueries.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "GameCommon/GameCameraService.h"
#include "Renderables/ModelPrimitive.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "LightMeshAssembler.h"
#include "LightingPawnRepository.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;

DEFINE_RTTI(GameCommon, DeferredRendererService, SceneRendererService);

DeferredRendererService::DeferredRendererService(ServiceManager* mngr,
    const std::shared_ptr<GameSceneService>& scene_service, const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<RendererManager>& renderer_manager,
    const std::shared_ptr<DeferredRendererServiceConfiguration>& configuration) : SceneRendererService(mngr, scene_service, camera_service, renderer_manager, configuration)
{
    m_configuration = configuration;
    LightVolumePawn::setDefaultVisualTech(m_configuration->visualTechniqueNameForCameraDefault());
    LightVolumePawn::setInsideVisualTech(m_configuration->visualTechniqueNameForCameraInside());

    m_lightMeshAssembler = nullptr;
    m_lightingPawns = nullptr;
}

DeferredRendererService::~DeferredRendererService()
{
    m_configuration = nullptr;
    m_lightingPawns = nullptr;
    m_lightMeshAssembler = nullptr;
}

ServiceResult DeferredRendererService::onInit()
{
    m_onPrimaryRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { onPrimaryRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetResized = std::make_shared<EventSubscriber>([=](auto e) { onPrimaryRenderTargetResized(e); });
    EventPublisher::subscribe(typeid(RenderTargetResized), m_onPrimaryRenderTargetResized);

    m_onLightCreated = std::make_shared<EventSubscriber>([=](auto e) { onLightCreatedOrConstituted(e); });
    EventPublisher::subscribe(typeid(SpatialCreated), m_onLightCreated);
    m_onLightConstituted = std::make_shared<EventSubscriber>([=](auto e) { onLightCreatedOrConstituted(e); });
    EventPublisher::subscribe(typeid(SpatialConstituted), m_onLightConstituted);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);

    m_lightMeshAssembler = std::make_shared<LightMeshAssembler>(m_configuration);
    m_lightMeshAssembler->registerHandlers();
    m_lightingPawns = std::make_shared<LightingPawnRepository>();
    m_lightingPawns->registerHandlers();

    return SceneRendererService::onInit();
}

ServiceResult DeferredRendererService::onTerm()
{
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetCreated = nullptr;
    EventPublisher::unsubscribe(typeid(RenderTargetResized), m_onPrimaryRenderTargetResized);
    m_onPrimaryRenderTargetResized = nullptr;

    EventPublisher::unsubscribe(typeid(SpatialCreated), m_onLightCreated);
    m_onLightCreated = nullptr;
    EventPublisher::unsubscribe(typeid(SpatialConstituted), m_onLightConstituted);
    m_onLightConstituted = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;

    m_lightMeshAssembler->unregisterHandlers();
    m_lightMeshAssembler = nullptr;
    m_lightingPawns->unregisterHandlers();
    m_lightingPawns = nullptr;

    return SceneRendererService::onTerm();
}

void DeferredRendererService::createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    auto rendererManager = m_rendererManager.lock();
    IRendererPtr renderer = std::make_shared<DeferredRenderer>(renderer_name);
    error er = rendererManager->insertRenderer(renderer_name, renderer);
    if (er) return;

    RenderTargetPtr primaryTarget = rendererManager->getPrimaryRenderTarget();
    if (!primaryTarget)
    {
        rendererManager->createRenderTarget(target_name, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
        primaryTarget = rendererManager->getPrimaryRenderTarget();
    }
    if (FATAL_LOG_EXPR(!primaryTarget)) return;

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, IRenderer>(rendererManager->getRenderer(renderer_name));
    m_renderer.lock()->selectRendererTechnique(m_configuration->deferredRendererTechniqueName());
    m_renderer.lock()->setRenderTarget(primaryTarget);
    if (primaryTarget->getBackSurface() && primaryTarget->getDepthStencilSurface())
    {
        createGBuffer(primaryTarget);
    }
}

void DeferredRendererService::destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->destroyRenderTarget(target_name);
    m_rendererManager.lock()->destroyRenderTarget(m_configuration->gbufferTargetName());
    m_rendererManager.lock()->destroyRenderer(renderer_name);
}

void DeferredRendererService::prepareGameScene()
{
    SceneRendererService::prepareGameScene();
}

void DeferredRendererService::createGBuffer(const RenderTargetPtr& primary_target)
{
    assert(primary_target);
    assert(!m_rendererManager.expired());
    assert(m_configuration);

    const auto& [width, height] = primary_target->getDimension();

    m_rendererManager.lock()->createRenderTarget(m_configuration->gbufferTargetName(), m_configuration->gbufferTextureId(), Graphics::MultiBackSurfaceSpecification(m_configuration->gbufferSurfaceName(), MathLib::Dimension<unsigned>{ width, height }, static_cast<unsigned>(m_configuration->gbufferFormats().size()), m_configuration->gbufferFormats()), m_configuration->gbufferDepthName(), primary_target->getDepthStencilSurface(), m_configuration->gbufferUsages());
    m_gBuffer = m_rendererManager.lock()->getRenderTarget(m_configuration->gbufferTargetName());
    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->attachGBufferTarget(m_gBuffer.lock());
    }
    bindGBufferToPendingLights();
}

void DeferredRendererService::onPrimaryRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    const auto& primaryTarget = ev->renderTarget();
    if (!primaryTarget) return;

    createGBuffer(primaryTarget);
}

void DeferredRendererService::onPrimaryRenderTargetResized(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    const auto target = ev->renderTarget();
    if ((!target) || (!target->isPrimary())) return;

    if ((!m_gBuffer.expired()) && (m_gBuffer.lock()->getDimension() != target->getDimension())) m_gBuffer.lock()->resize(target->getDimension());
}

void DeferredRendererService::onLightCreatedOrConstituted(const IEventPtr& e)
{
    if (!m_lightingPawns) return;
    if (!e) return;
    std::shared_ptr<Light> light;
    if (auto ev = std::dynamic_pointer_cast<SpatialCreated, IEvent>(e))
    {
        if (ev->rtti().isDerived(Light::TYPE_RTTI))
        {
            light = std::dynamic_pointer_cast<Light>(ev->spatial());
        }
    }
    else if (auto ev = std::dynamic_pointer_cast<SpatialConstituted, IEvent>(e))
    {
        if (ev->rtti().isDerived(Light::TYPE_RTTI))
        {
            light = std::dynamic_pointer_cast<Light>(ev->spatial());
        }
    }
    if (!light) return;
    if (m_gBuffer.expired())
    {
        m_pendingLightsOfGBufferBind.push_back(light);
        return;
    }
    if (light->info().lightType() == LightInfo::LightType::Ambient)
    {
        m_lightingPawns->createAmbientLightPawn(light);
    }
    else if (light->info().lightType() == LightInfo::LightType::SunLight)
    {
        m_lightingPawns->createSunLightPawn(light);
    }
    else if (light->info().lightType() == LightInfo::LightType::Point)
    {
        m_lightingPawns->createPointLightPawn(light);
    }
}

void DeferredRendererService::onLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    m_lightingPawns->removeLightingPawn(ev->lightId());
}

void DeferredRendererService::bindGBufferToPendingLights()
{
    if (m_pendingLightsOfGBufferBind.empty()) return;
    for (const auto& light : m_pendingLightsOfGBufferBind)
    {
        if (!light) continue;
        if (light->info().lightType() == LightInfo::LightType::Ambient)
        {
            m_lightingPawns->createAmbientLightPawn(light);
        }
        else if (light->info().lightType() == LightInfo::LightType::SunLight)
        {
            m_lightingPawns->createSunLightPawn(light);
        }
        else if (light->info().lightType() == LightInfo::LightType::Point)
        {
            m_lightingPawns->createPointLightPawn(light);
        }
    }
    m_pendingLightsOfGBufferBind.clear();
}

