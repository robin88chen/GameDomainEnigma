#include "DeferredRendererService.h"
#include "DeferredRendererServiceConfiguration.h"
#include "Controllers/GraphicMain.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/DeferredRenderer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "GameEngine/EffectDtoHelper.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Renderer/RenderablePrimitiveRequests.h"
#include "Renderer/RenderablePrimitiveResponses.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Renderer/RendererEvents.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

using error = std::error_code;

DEFINE_RTTI(GameCommon, DeferredRendererService, SceneRendererService);

DeferredRendererService::DeferredRendererService(ServiceManager* mngr,
    const std::shared_ptr<GameSceneService>& scene_service, const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
    std::unique_ptr<DeferredRendererServiceConfiguration> configuration) : SceneRendererService(mngr, scene_service, camera_service, renderer_manager)
{
    m_configuration = std::move(configuration);
    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
}

DeferredRendererService::~DeferredRendererService()
{
    m_configuration = nullptr;
    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
    m_lightVolumes.clear();
}

ServiceResult DeferredRendererService::OnInit()
{
    m_onPrimaryRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { OnPrimaryRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoCreated(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoDeleted(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoUpdated(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);

    m_onBuildPrimitiveResponse = std::make_shared<ResponseSubscriber>([=](auto e) { OnBuildPrimitiveResponse(e); });
    ResponseBus::Subscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);

    return SceneRendererService::OnInit();
}

ServiceResult DeferredRendererService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;

    ResponseBus::Unsubscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);
    m_onBuildPrimitiveResponse = nullptr;

    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
    m_lightVolumes.clear();

    return SceneRendererService::OnTerm();
}

void DeferredRendererService::CreateSceneRenderSystem(const std::string& renderer_name, const std::string& target_name, bool is_primary)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    auto rendererManager = m_rendererManager.lock();
    Engine::IRendererPtr renderer = std::make_shared<Renderer::DeferredRenderer>(renderer_name);
    error er = rendererManager->InsertRenderer(renderer_name, renderer);
    if (er) return;

    RenderTargetPtr primaryTarget = rendererManager->GetPrimaryRenderTarget();
    if (!primaryTarget)
    {
        rendererManager->CreateRenderTarget(target_name, RenderTarget::PrimaryType::IsPrimary);
        primaryTarget = rendererManager->GetPrimaryRenderTarget();
    }
    if (FATAL_LOG_EXPR(!primaryTarget)) return;

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(rendererManager->GetRenderer(renderer_name));
    m_renderer.lock()->SelectRendererTechnique(m_configuration->DeferredRendererTechniqueName());
    if (primaryTarget)
    {
        m_renderer.lock()->SetRenderTarget(primaryTarget);
    }
}

void DeferredRendererService::DestroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->DestroyRenderTarget(target_name);
    m_rendererManager.lock()->DestroyRenderTarget(m_configuration->GbufferTargetName());
    m_rendererManager.lock()->DestroyRenderer(renderer_name);
}

void DeferredRendererService::PrepareGameScene()
{
    if (!m_renderer.expired())
    {
        if (m_ambientLightQuad) m_ambientLightQuad->InsertToRendererWithTransformUpdating(m_renderer.lock(),
            MathLib::Matrix4::IDENTITY, m_ambientQuadRenderState.ToLightingState());
        if (m_sunLightQuad) m_sunLightQuad->InsertToRendererWithTransformUpdating(m_renderer.lock(),
            MathLib::Matrix4::IDENTITY, m_sunLightQuadRenderState.ToLightingState());
    }
    SceneRendererService::PrepareGameScene();
}

RenderTargetPtr DeferredRendererService::CreateGBuffer(unsigned width, unsigned height, const Graphics::IDepthStencilSurfacePtr& depth) const
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->CreateRenderTarget(m_configuration->GbufferTargetName(), RenderTarget::PrimaryType::NotPrimary);
    RenderTargetPtr gbuffer = m_rendererManager.lock()->GetRenderTarget(m_configuration->GbufferTargetName());
    if (gbuffer)
    {
        const std::vector<Graphics::GraphicFormat> formats =
        {
            Graphics::GraphicFormat(Graphics::GraphicFormat::FMT_A16B16G16R16F),  // normal
            Graphics::GraphicFormat(Graphics::GraphicFormat::FMT_A16B16G16R16F),     // diffuse material
            Graphics::GraphicFormat(Graphics::GraphicFormat::FMT_A16B16G16R16F),     // specular material r,g,b, & shininess
            Graphics::GraphicFormat(Graphics::GraphicFormat::FMT_R32F),          // depth
        };
        gbuffer->InitMultiBackSurface(m_configuration->GbufferSurfaceName(), MathLib::Dimension{ width, height }, 4, formats);
        gbuffer->SetGBufferDepthMapIndex(3);
        gbuffer->ShareDepthStencilSurface(m_configuration->GbufferDepthName(), depth);
    }
    return gbuffer;
}

void DeferredRendererService::OnPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::PrimaryRenderTargetCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    auto primaryTarget = ev->GetRenderTarget();
    if (!primaryTarget) return;

    auto [width, height] = primaryTarget->GetDimension();

    // create GBuffer to substitute
    RenderTargetPtr gbuffer = CreateGBuffer(width, height, primaryTarget->GetDepthStencilSurface());
    if (FATAL_LOG_EXPR(!gbuffer)) return;

    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->AttachGBufferTarget(gbuffer);
    }
}

void DeferredRendererService::OnLightInfoCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoCreated, Frameworks::IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        CreateAmbientLightQuad(ev->GetLight()->Info());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        CreateSunLightQuad(ev->GetLight()->Info());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        CreatePointLightVolume(ev->GetLight()->Info());
    }
}

void DeferredRendererService::OnLightInfoDeleted(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoDeleted, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        m_ambientLightQuad = nullptr;
    }
    else if (ev->GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        m_sunLightQuad = nullptr;
    }
    else if (ev->GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        DeletePointLightVolume(ev->GetLightName());
    }
}

void DeferredRendererService::OnLightInfoUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoUpdated, Frameworks::IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        UpdateAmbientLightQuad(ev->GetLight()->Info(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        UpdateSunLightQuad(ev->GetLight()->Info(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        UpdatePointLightVolume(ev->GetLight()->Info(), ev->GetNotifyCode());
    }
}

void DeferredRendererService::OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    const auto res = std::dynamic_pointer_cast<Renderer::BuildRenderablePrimitiveResponse, Frameworks::IResponse>(r);
    if (!res) return;
    if (res->GetRequestRuid() == m_ambientQuadRequester)
    {
        m_ambientLightQuad = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(res->GetPrimitive());
    }
}

void DeferredRendererService::CreateAmbientLightQuad(const SceneGraph::LightInfo& lit)
{
    std::string quad_geo_name = std::string("deferred_ambient_quad.geo");
    SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.XYQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .TextureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->AmbientPassFxFileName());
    eff_dto_helper.FilenameAtPath(m_configuration->AmbientPassFxFileName());

    EffectTextureMapDtoHelper tex_dto_helper;
    tex_dto_helper.TextureMapping("", "", m_configuration->GbufferTargetName(), 1, m_configuration->GbufferDiffuseSemantic())
        .TextureMapping("", "", m_configuration->GbufferTargetName(), 3, m_configuration->GbufferDepthSemantic());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = quad_geo_name;
    mesh_dto.GeometryName() = quad_geo_name;
    mesh_dto.TheGeometry() = quad_dto_helper.ToGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.ToGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto_helper.ToGenericDto());

    auto mesh_policy = mesh_dto.ConvertToPolicy(nullptr);
    auto request = std::make_shared<Renderer::RequestBuildRenderablePrimitive>(mesh_policy);
    m_ambientQuadRequester = request->GetRuid();
    RequestBus::Post(request);
}

void DeferredRendererService::CreateSunLightQuad(const SceneGraph::LightInfo& lit)
{

}

void DeferredRendererService::CreatePointLightVolume(const SceneGraph::LightInfo& lit)
{

}

void DeferredRendererService::DeletePointLightVolume(const std::string& name)
{

}

void DeferredRendererService::UpdateAmbientLightQuad(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{

}

void DeferredRendererService::UpdatePointLightVolume(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{

}

void DeferredRendererService::UpdateSunLightQuad(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{

}
