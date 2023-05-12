#include "DeferredRendererService.h"
#include "DeferredRendererServiceConfiguration.h"
#include "Controllers/GraphicMain.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/DeferredRenderer.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;

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
