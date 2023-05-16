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
#include "Frameworks/RequestBus.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;

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
    return SceneRendererService::OnInit();
}

ServiceResult DeferredRendererService::OnTerm()
{
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

    auto [width, height] = primaryTarget->GetDimension();

    // create GBuffer to substitute
    RenderTargetPtr gbuffer = CreateGBuffer(width, height, primaryTarget->GetDepthStencilSurface());
    if (FATAL_LOG_EXPR(!gbuffer)) return;

    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->AttachGBufferTarget(gbuffer);
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
    RequestBus::Post(request);
}
