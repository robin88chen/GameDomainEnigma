#include "Renderer.h"
#include "RendererErrors.h"
#include "RenderTarget.h"
#include "SceneGraph/Camera.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::Renderer;

Renderer::Renderer(const std::string& name) : IRenderer(name)
{
    m_renderPacksArray[static_cast<size_t>(RenderListID::Overlay)].enableSortBeforeDraw(false);
    m_renderPacksArray[static_cast<size_t>(RenderListID::DeferredLighting)].enableSortBeforeDraw(false);
    m_renderPacksArray[static_cast<size_t>(RenderListID::OffSurface)].sortByDistance();
    m_stampBitMask = 0;
}

Renderer::~Renderer()
{
    flushAll();
}

error Renderer::setRenderTarget(const std::shared_ptr<RenderTarget>& target)
{
    m_target = target;
    return ErrorCode::ok;
}

std::shared_ptr<RenderTarget> Renderer::getRenderTarget() const
{
    if (!m_target.expired()) return m_target.lock();
    return nullptr;
}

error Renderer::changeClearingProperty(const RenderTargetClearChangingProperty& prop)
{
    if (!getRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = getRenderTarget()->changeClearingProperty(prop);
    return er;
}

error Renderer::clearRenderTarget()
{
    if (!getRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = getRenderTarget()->clear();
    return er;
}

error Renderer::flip()
{
    if (!getRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = getRenderTarget()->flip();
    return er;
}

error Renderer::insertRenderElement(const std::shared_ptr<RenderElement>& element,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lighting, RenderListID list_id)
{
    assert(element);
    error er;
    if ((list_id == RenderListID::OffSurface) && (!m_associatedCamera.expired()))
    {
        er = m_renderPacksArray[static_cast<size_t>(list_id)].
            insertRenderElement(element, mxWorld, m_associatedCamera.lock()->location(), lighting, m_stampBitMask);
    }
    else
    {
        er = m_renderPacksArray[static_cast<size_t>(list_id)].
            insertRenderElement(element, mxWorld, lighting, m_stampBitMask);
    }
    return er;
}

error Renderer::removeRenderElement(const std::shared_ptr<RenderElement>& element, RenderListID list_id)
{
    return m_renderPacksArray[static_cast<size_t>(list_id)].removeRenderElement(element, m_stampBitMask);
}

error Renderer::beginScene()
{
    if (!m_target.expired())
    {
        m_target.lock()->bind();
        m_target.lock()->clear();
        m_target.lock()->bindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_target.lock()->getViewPort());
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

error Renderer::beginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj)
{
    if (!m_target.expired())
    {
        m_target.lock()->bind();
        m_target.lock()->clear();
        m_target.lock()->bindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_target.lock()->getViewPort());
    }
    Engine::MaterialVariableMap::useCameraParameter(camera_loc, mxView, mxProj);
    Graphics::IGraphicAPI::instance()->beginScene();
    return ErrorCode::ok;
}

error Renderer::prepareScene(const SceneGraph::VisibleSet& visible_set,
    SceneGraph::Spatial::SpatialFlags accept_filter, SceneGraph::Spatial::SpatialFlags reject_filter)
{
    const size_t count = visible_set.getCount();
    if (count <= 0) return ErrorCode::ok;
    const SceneGraph::VisibleSet::SpatialVector& spatial_array = visible_set.GetObjectSet();
    for (const auto& sp : spatial_array)
    {
        if (!sp->isRenderable()) continue;  // 有很多不需要render的node, 先檢查效率較好
        if (!sp->testSpatialFlag(accept_filter)) continue;
        if (sp->testSpatialFlag(reject_filter)) continue;
        sp->insertToRenderer(shared_from_this());
    }
    return ErrorCode::ok;
}

error Renderer::drawScene()
{
    for (size_t i = 0; i < m_renderPacksArray.size(); i++)
    {
        if (!m_renderPacksArray[i].hasElements()) continue;

        if (const error er = m_renderPacksArray[i].draw(m_stampBitMask, m_rendererTechniqueName)) return er;
        // ui 的 render 跟順序有關，除了不做排序外，element的cache也有可能會破壞順序，所以要多花時間做flush
        if (i == static_cast<size_t>(RenderListID::Overlay)) m_renderPacksArray[i].flushAll(m_stampBitMask);
    }

    return ErrorCode::ok;
}

error Renderer::endScene()
{
    Graphics::IGraphicAPI::instance()->endScene();
    return ErrorCode::ok;
}

void Renderer::flushAll()
{
    for (auto& pack : m_renderPacksArray)
    {
        pack.flushAll(m_stampBitMask);
    }
}

void Renderer::setAssociatedCamera(const std::shared_ptr<SceneGraph::Camera>& camera)
{
    m_associatedCamera = camera;
}

void Renderer::enableSortBeforeDraw(RenderListID list_id, bool flag)
{
    if (static_cast<size_t>(list_id) >= static_cast<size_t>(RenderListID::Count)) return;
    m_renderPacksArray[static_cast<size_t>(list_id)].enableSortBeforeDraw(flag);
}

void Renderer::selectRendererTechnique(const std::string& techniqueName)
{
    m_rendererTechniqueName = techniqueName;
}
