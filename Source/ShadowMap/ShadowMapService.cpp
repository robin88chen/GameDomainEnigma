#include "ShadowMapService.h"
#include "ShadowMapServiceConfiguration.h"
#include "SunLightCamera.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/Spatial.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "GameCommon/GameCameraService.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;

ShadowMapService::ShadowMapService(ServiceManager* manager, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
                                   const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
                                   const std::shared_ptr<RendererManager>& renderer_manager, std::unique_ptr<ShadowMapServiceConfiguration> configuration)
    : ISystemService(manager)
{
    m_needTick = true;
    m_configuration = std::move(configuration);
    m_sceneService = scene_service;
    m_cameraService = camera_service;
    m_rendererManager = renderer_manager;
}

ShadowMapService::~ShadowMapService()
{
}

ServiceResult ShadowMapService::OnInit()
{
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoCreated(e); });
    EventPublisher::Subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoDeleted(e); });
    EventPublisher::Subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoUpdated(e); });
    EventPublisher::Subscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    return ServiceResult::Complete;
}

ServiceResult ShadowMapService::OnTick()
{
    return ServiceResult::Pendding;
}

ServiceResult ShadowMapService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::Unsubscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    return ServiceResult::Complete;
}

void ShadowMapService::CreateShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->CreateRenderer(renderer_name);
    m_rendererManager.lock()->CreateRenderTarget(target_name, RenderTarget::PrimaryType::NotPrimary, { Graphics::RenderTextureUsage::ShadowMap });

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(m_rendererManager.lock()->GetRenderer(renderer_name));
    m_renderer.lock()->SelectRendererTechnique(m_configuration->ShadowMapTechniqueName());

    auto target = m_rendererManager.lock()->GetRenderTarget(target_name);
    target->InitBackSurface(m_configuration->ShadowMapSurfaceName(), m_configuration->ShadowMapDimension(), Graphics::GraphicFormat::FMT_R32F);
    target->InitDepthStencilSurface(m_configuration->ShadowMapDepthName(), m_configuration->ShadowMapDimension(),
        Graphics::IGraphicAPI::Instance()->GetDepthSurfaceFormat());
    m_renderer.lock()->SetRenderTarget(target);
    target->ChangeClearingProperty(RenderTargetClearChangingProperty{ MathLib::ColorRGBA(1.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0, std::nullopt });

    if (m_sunLightCamera)
    {
        m_renderer.lock()->SetAssociatedCamera(m_sunLightCamera);
    }
}

void ShadowMapService::DestroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->DestroyRenderTarget(target_name);
    m_rendererManager.lock()->DestroyRenderer(renderer_name);
}

void ShadowMapService::PrepareShadowScene()
{
    if ((!m_renderer.expired()) && (!m_sceneService.expired()) && (m_sceneService.lock()->GetSceneCuller()))
    {
        m_renderer.lock()->PrepareScene(m_sceneService.lock()->GetSceneCuller()->GetVisibleSet(),
            Spatial::SpatialBit::Spatial_ShadowCaster);
    }
}

void ShadowMapService::RenderShadowScene()
{
    if (m_renderer.expired()) return;
    m_renderer.lock()->ClearRenderTarget();
    m_renderer.lock()->BeginScene();
    m_renderer.lock()->DrawScene();
    m_renderer.lock()->EndScene();
}

void ShadowMapService::OnLightInfoCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoCreated, IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() != LightInfo::LightType::SunLight) return;
    CreateSunLightCamera(ev->GetLight());
}

void ShadowMapService::OnLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    if (ev->GetLight()->Info().GetLightType() != LightInfo::LightType::SunLight) return;
    if (!m_renderer.expired())
    {
        m_renderer.lock()->SetAssociatedCamera(nullptr);
    }
    m_sunLightCamera = nullptr;
}

void ShadowMapService::OnLightInfoUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoUpdated, IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() != LightInfo::LightType::SunLight) return;
    if (m_sunLightCamera)
    {
        m_sunLightCamera->SetSunLightDir(ev->GetLight()->GetLightDirection());
    }
}

void ShadowMapService::CreateSunLightCamera(const std::shared_ptr<Light>& lit)
{
    assert(!m_cameraService.expired());
    m_sunLightCamera = std::make_shared<SunLightCamera>(m_configuration->SunLightCameraName());
    MathLib::Vector3 vecSunDir = MathLib::Vector3(-1.0f, -1.0f, 0.0f);
    if (lit) vecSunDir = lit->GetLightDirection();
    m_sunLightCamera->SetSunLightDir(vecSunDir);
    if (auto cam = m_cameraService.lock()->GetPrimaryCamera())
    {
        m_sunLightCamera->SetViewerCamera(cam);
    }
    if (!m_renderer.expired())
    {
        m_renderer.lock()->SetAssociatedCamera(m_sunLightCamera);
    }
}
