#include "DeferredRenderer.h"
#include "RenderTarget.h"
#include "RendererErrors.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::Renderer;

constexpr const char* gbuffer_depth_stencil_name = "GBufferDepthStencil";

DeferredRenderer::DeferredRenderer(const std::string& name) : Renderer(name), m_gbufferClearColor(0.0f, 0.0f, 0.0f, 0.0f)
{
}

DeferredRenderer::~DeferredRenderer()
{
}

error DeferredRenderer::ChangeClearingProperty(const RenderTargetClearChangingProperty& prop)
{
    RenderTargetClearChangingProperty primary_prop = prop;
    if (primary_prop.m_clearingBits)
    {
        primary_prop.m_clearingBits.value() &= ~RenderTargetClear::DepthBuffer;  // scene target與 GBuffer共用 depth, 所以不用清depth
    }
    error er = Renderer::ChangeClearingProperty(primary_prop);
    if (er) return er;

    RenderTargetClearChangingProperty gbuffer_prop = prop;
    gbuffer_prop.m_color = m_gbufferClearColor;
    if (!m_gbufferTarget.expired())
    {
        er = m_gbufferTarget.lock()->ChangeClearingProperty(gbuffer_prop);
    }
    return er;
}

error DeferredRenderer::ClearRenderTarget()
{
    if (m_target.expired()) return ErrorCode::nullRenderTarget;

    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->Clear();
    }

    return m_target.lock()->Clear();
}

error DeferredRenderer::BeginScene()
{
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->Bind();
        m_gbufferTarget.lock()->BindViewPort();
        Engine::MaterialVariableMap::UseViewPortDimension(m_gbufferTarget.lock()->GetViewPort());
    }
    std::shared_ptr<SceneGraph::Camera> camera;
    if ((!m_associatedCamera.expired()) && (m_associatedCamera.lock()->GetCullingFrustum()))
    {
        camera = m_associatedCamera.lock();
    }
    if (camera)
    {
        Engine::MaterialVariableMap::UseCameraParameter(camera->GetLocation(),
            camera->GetViewTransform(), camera->GetProjectionTransform());
    }
    Graphics::IGraphicAPI::Instance()->BeginScene();
    return ErrorCode::ok;
}

error DeferredRenderer::BeginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj)
{
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->Bind();
        m_gbufferTarget.lock()->BindViewPort();
        Engine::MaterialVariableMap::UseViewPortDimension(m_gbufferTarget.lock()->GetViewPort());
    }
    Engine::MaterialVariableMap::UseCameraParameter(camera_loc, mxView, mxProj);

    Graphics::IGraphicAPI::Instance()->BeginScene();
    return ErrorCode::ok;
}

error DeferredRenderer::DrawScene()
{
    for (size_t i = 0; i < m_renderPacksArray.size(); i++)
    {
        if (i == static_cast<size_t>(RenderListID::DeferredLighting)) // switch render target for deferred lighting and final blending
        {
            Graphics::IGraphicAPI::Instance()->EndScene();  // end previous stage

            // in this stage, we need to render to the primary target using g-buffer
            if (!m_target.expired())  // that's the primary target
            {
                error er_bind = m_target.lock()->Bind();
                if (er_bind) return er_bind;
                er_bind = m_target.lock()->BindViewPort();
                if (er_bind) return er_bind;
                Engine::MaterialVariableMap::UseViewPortDimension(m_target.lock()->GetViewPort());
            }

            Graphics::IGraphicAPI::Instance()->BeginScene();
        }
        if (!m_renderPacksArray[i].HasElements()) continue;
        error er = m_renderPacksArray[i].Draw(m_stampBitMask, m_rendererTechniqueName);
        if (er) return er;
        // ui 的 render 跟順序有關，除了不做排序外，element的cache也有可能會破壞順序，所以要多花時間做flush
        if (i == static_cast<size_t>(RenderListID::Overlay)) m_renderPacksArray[i].FlushAll(m_stampBitMask);
    }

    return ErrorCode::ok;
}

error DeferredRenderer::AttachGBufferTarget(const std::shared_ptr<RenderTarget>& gbuffer)
{
    m_gbufferTarget = gbuffer;
    RenderTargetClearChangingProperty prop;
    prop.m_color = m_gbufferClearColor;
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->ChangeClearingProperty(prop);
    }
    return ErrorCode::ok;
}

void DeferredRenderer::ReShareDepthStencilSurface()
{
    if (FATAL_LOG_EXPR((m_target.expired()) || (m_gbufferTarget.expired()))) return;
    if (FATAL_LOG_EXPR(m_target.lock()->GetDepthStencilSurface() == nullptr)) return;
    m_gbufferTarget.lock()->ShareDepthStencilSurface(gbuffer_depth_stencil_name, m_target.lock()->GetDepthStencilSurface());
}

