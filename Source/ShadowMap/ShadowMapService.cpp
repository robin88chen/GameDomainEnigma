#include "ShadowMapService.h"
#include "ShadowMapServiceConfiguration.h"
#include "SunLightCamera.h"
#include "SpatialShadowFlags.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/Spatial.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "GameCommon/GameCameraService.h"
#include "GameEngine/MaterialVariableMap.h"
#include "SceneGraph/Pawn.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/ModelPrimitive.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::Renderer;

DEFINE_RTTI(ShadowMap, ShadowMapService, ISystemService);

ShadowMapService::ShadowMapService(ServiceManager* manager, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
    const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
    const std::shared_ptr<RendererManager>& renderer_manager, const std::shared_ptr<ShadowMapServiceConfiguration>& configuration)
    : ISystemService(manager)
{
    m_needTick = true;
    m_configuration = configuration;
    m_sceneService = scene_service;
    m_cameraService = camera_service;
    m_rendererManager = renderer_manager;
}

ShadowMapService::~ShadowMapService()
{
    m_configuration = nullptr;
}

ServiceResult ShadowMapService::onInit()
{
    subscribeEvents();
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->lightViewProjSemantic(), assignLightViewProjectionTransform);
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->shadowMapDimensionSemantic(), assignShadowMapDimension);
    return ServiceResult::Complete;
}

ServiceResult ShadowMapService::onTick()
{
    if ((!m_sceneService.expired()) && (m_sunLightCamera))
    {
        m_sunLightCamera->calcLightCameraSystemMatrix(m_sceneService.lock()->getSceneCuller());
        m_lightViewProjectionTransform = m_sunLightCamera->getLightViewProjMatrix();
    }
    return ServiceResult::Pendding;
}

ServiceResult ShadowMapService::onTerm()
{
    m_sunLightCamera = nullptr;

    unsubscribeEvents();
    return ServiceResult::Complete;
}

void ShadowMapService::subscribeEvents()
{
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoCreated(e); });
    EventPublisher::subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoUpdated(e); });
    EventPublisher::subscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onPawnConstituted = std::make_shared<EventSubscriber>([=](auto e) { onPawnConstituted(e); });
    EventPublisher::subscribe(typeid(SpatialConstituted), m_onPawnConstituted);
}

void ShadowMapService::unsubscribeEvents()
{
    EventPublisher::unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    EventPublisher::unsubscribe(typeid(SpatialConstituted), m_onPawnConstituted);
    m_onPawnConstituted = nullptr;
}

void ShadowMapService::createShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->createRenderer(renderer_name);
    m_rendererManager.lock()->createRenderTarget(target_name, Graphics::BackSurfaceSpecification(m_configuration->shadowMapSurfaceName(), m_configuration->shadowMapDimension(), Graphics::GraphicFormat::FMT_R32F), Graphics::DepthStencilSurfaceSpecification(m_configuration->shadowMapDepthName(), m_configuration->shadowMapDimension(),
        Graphics::IGraphicAPI::instance()->getDepthSurfaceFormat()), { Graphics::RenderTextureUsage::ShadowMap });

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(m_rendererManager.lock()->getRenderer(renderer_name));
    m_renderer.lock()->selectRendererTechnique(m_configuration->shadowMapTechniqueName());

    m_shadowMapRenderTarget = m_rendererManager.lock()->getRenderTarget(target_name);
    //m_shadowMapRenderTarget.lock()->initBackSurface(Graphics::BackSurfaceSpecification(m_configuration->shadowMapSurfaceName(), m_configuration->shadowMapDimension(), Graphics::GraphicFormat::FMT_R32F));
    //m_shadowMapRenderTarget.lock()->initDepthStencilSurface(Graphics::DepthStencilSurfaceSpecification(m_configuration->shadowMapDepthName(), m_configuration->shadowMapDimension(),        Graphics::IGraphicAPI::instance()->getDepthSurfaceFormat()));
    m_renderer.lock()->setRenderTarget(m_shadowMapRenderTarget.lock());
    m_shadowMapRenderTarget.lock()->changeClearingProperty(RenderTargetClearChangingProperty{ MathLib::ColorRGBA(1.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0, std::nullopt });

    if (m_sunLightCamera)
    {
        m_renderer.lock()->setAssociatedCamera(m_sunLightCamera);
    }

    m_shadowMapDimensionBiasDensity[0] = static_cast<float>(m_configuration->shadowMapDimension().m_width);
    m_shadowMapDimensionBiasDensity[1] = static_cast<float>(m_configuration->shadowMapDimension().m_height);
    m_shadowMapDimensionBiasDensity[2] = m_configuration->shadowMapDepthBias();
    m_shadowMapDimensionBiasDensity[3] = m_configuration->shadowMapDensity();
}

void ShadowMapService::destroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->destroyRenderTarget(target_name);
    m_rendererManager.lock()->destroyRenderer(renderer_name);
}

void ShadowMapService::prepareShadowScene()
{
    if ((!m_renderer.expired()) && (!m_sceneService.expired()) && (m_sceneService.lock()->getSceneCuller()))
    {
        m_renderer.lock()->prepareScene(m_sceneService.lock()->getSceneCuller()->getVisibleSet(),
            SpatialShadowFlags::SpatialBit::Spatial_ShadowCaster);
    }
}

void ShadowMapService::renderShadowScene()
{
    if (m_renderer.expired()) return;
    m_renderer.lock()->clearRenderTarget();
    m_renderer.lock()->beginScene();
    m_renderer.lock()->drawScene();
    m_renderer.lock()->endScene();
}

void ShadowMapService::onLightInfoCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoCreated, IEvent>(e);
    if ((!ev) || (!ev->light())) return;
    if (ev->light()->info().lightType() != LightInfo::LightType::SunLight) return;
    createSunLightCamera(ev->light());
}

void ShadowMapService::onLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    if (ev->lightType() != LightInfo::LightType::SunLight) return;
    deleteSunLightCamera();
}

void ShadowMapService::onLightInfoUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoUpdated, IEvent>(e);
    if ((!ev) || (!ev->light())) return;
    if (ev->light()->info().lightType() != LightInfo::LightType::SunLight) return;
    updateSunLightDirection(ev->light()->getLightDirection());
}

void ShadowMapService::onPawnConstituted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SpatialConstituted, IEvent>(e);
    if (!ev) return;
    const auto pawn = std::dynamic_pointer_cast<Pawn>(ev->spatial());
    if (!pawn) return;
    if ((pawn->testSpatialFlag(SpatialShadowFlags::SpatialBit::Spatial_ShadowCaster))
        || (pawn->testSpatialFlag(SpatialShadowFlags::SpatialBit::Spatial_ShadowReceiver)))
    {
        bindShadowMapToPawn(pawn);
    }
}

void ShadowMapService::createSunLightCamera(const std::shared_ptr<Light>& lit)
{
    assert(!m_cameraService.expired());
    m_sunLightCamera = std::make_shared<SunLightCamera>(m_configuration->sunLightCameraId());
    MathLib::Vector3 vecSunDir = MathLib::Vector3(-1.0f, -1.0f, 0.0f);
    if (lit) vecSunDir = lit->getLightDirection();
    m_sunLightCamera->setSunLightDir(vecSunDir);
    if (auto cam = m_cameraService.lock()->primaryCamera())
    {
        m_sunLightCamera->setViewerCamera(cam);
    }
    if (!m_renderer.expired())
    {
        m_renderer.lock()->setAssociatedCamera(m_sunLightCamera);
    }
}

void ShadowMapService::deleteSunLightCamera()
{
    if (!m_renderer.expired())
    {
        m_renderer.lock()->setAssociatedCamera(nullptr);
    }
    m_sunLightCamera = nullptr;
}

void ShadowMapService::updateSunLightDirection(const MathLib::Vector3& dir)
{
    if (m_sunLightCamera)
    {
        m_sunLightCamera->setSunLightDir(dir);
    }
}

void ShadowMapService::bindShadowMapToPawn(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if (!pawn->getPrimitive()) return;
    if (const auto model = std::dynamic_pointer_cast<ModelPrimitive>(pawn->getPrimitive()))
    {
        const auto mesh_count = model->getMeshPrimitiveCount();
        for (unsigned i = 0; i < mesh_count; i++)
        {
            bindShadowMapToMesh(model->getMeshPrimitive(i));
        }
    }
    else if (const auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(pawn->getPrimitive()))
    {
        bindShadowMapToMesh(mesh);
    }
}

void ShadowMapService::bindShadowMapToMesh(const std::shared_ptr<MeshPrimitive>& mesh)
{
    if (!mesh) return;
    if (m_shadowMapRenderTarget.expired()) return;

    mesh->bindSemanticTexture({ m_configuration->shadowMapSemantic(), m_shadowMapRenderTarget.lock()->getRenderTargetTexture(), std::nullopt });
}

void ShadowMapService::assignLightViewProjectionTransform(Engine::EffectVariable& var)
{
    var.assignValue(m_lightViewProjectionTransform);
}

void ShadowMapService::assignShadowMapDimension(Engine::EffectVariable& var)
{
    var.assignValue(m_shadowMapDimensionBiasDensity);
}
