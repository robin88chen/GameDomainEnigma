#include "DeferredRenderer.h"
#include "RenderTarget.h"
#include "RendererErrors.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::Renderer;

DeferredRenderer::DeferredRenderer(const std::string& name) : Renderer(name), m_gbufferClearColor(0.0f, 0.0f, 0.0f, 0.0f)
{
}

DeferredRenderer::~DeferredRenderer()
{
}

error DeferredRenderer::changeClearingProperty(const RenderTargetClearChangingProperty& prop)
{
    RenderTargetClearChangingProperty primary_prop = prop;
    if (primary_prop.m_clearingBits)
    {
        primary_prop.m_clearingBits.value() &= ~RenderTargetClear::DepthBuffer;  // scene target與 GBuffer共用 depth, 所以不用清depth
    }
    error er = Renderer::changeClearingProperty(primary_prop);
    if (er) return er;

    RenderTargetClearChangingProperty gbuffer_prop = prop;
    gbuffer_prop.m_color = m_gbufferClearColor;
    if (!m_gbufferTarget.expired())
    {
        er = m_gbufferTarget.lock()->changeClearingProperty(gbuffer_prop);
    }
    return er;
}

error DeferredRenderer::clearRenderTarget()
{
    if (m_target.expired()) return ErrorCode::nullRenderTarget;

    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->clear();
    }

    return m_target.lock()->clear();
}

error DeferredRenderer::beginScene()
{
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->bind();
        m_gbufferTarget.lock()->clear();
        m_gbufferTarget.lock()->bindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_gbufferTarget.lock()->getViewPort());
    }
    std::shared_ptr<SceneGraph::Camera> camera;
    if (!m_associatedCamera.expired())
    {
        camera = m_associatedCamera.lock();
    }
    if (camera)
    {
        Engine::MaterialVariableMap::useCameraParameter(camera->location(),
            camera->viewTransform(), camera->projectionTransform());
    }
    Graphics::IGraphicAPI::instance()->beginScene();
    return ErrorCode::ok;
}

error DeferredRenderer::beginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj)
{
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->bind();
        m_gbufferTarget.lock()->clear();
        m_gbufferTarget.lock()->bindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_gbufferTarget.lock()->getViewPort());
    }
    Engine::MaterialVariableMap::useCameraParameter(camera_loc, mxView, mxProj);

    Graphics::IGraphicAPI::instance()->beginScene();
    return ErrorCode::ok;
}

error DeferredRenderer::drawScene()
{
    for (size_t i = 0; i < m_renderPacksArray.size(); i++)
    {
        if (i == static_cast<size_t>(RenderListID::DeferredLighting)) // switch render target for deferred lighting and final blending
        {
            Graphics::IGraphicAPI::instance()->endScene();  // end previous stage

            // in this stage, we need to render to the primary target using g-buffer
            if (!m_target.expired())  // that's the primary target
            {
                error er_bind = m_target.lock()->bind();
                if (er_bind) return er_bind;
                m_target.lock()->clear();
                er_bind = m_target.lock()->bindViewPort();
                if (er_bind) return er_bind;
                Engine::MaterialVariableMap::useViewPortDimension(m_target.lock()->getViewPort());
            }

            Graphics::IGraphicAPI::instance()->beginScene();
        }
        if (!m_renderPacksArray[i].hasElements()) continue;
        error er = m_renderPacksArray[i].draw(m_stampBitMask, m_rendererTechniqueName);
        if (er) return er;
        // ui 的 render 跟順序有關，除了不做排序外，element的cache也有可能會破壞順序，所以要多花時間做flush
        if (i == static_cast<size_t>(RenderListID::Overlay)) m_renderPacksArray[i].flushAll(m_stampBitMask);
    }

    return ErrorCode::ok;
}

error DeferredRenderer::attachGBufferTarget(const std::shared_ptr<RenderTarget>& gbuffer)
{
    m_gbufferTarget = gbuffer;
    RenderTargetClearChangingProperty prop;
    prop.m_color = m_gbufferClearColor;
    if (!m_gbufferTarget.expired())
    {
        m_gbufferTarget.lock()->changeClearingProperty(prop);
    }
    return ErrorCode::ok;
}

