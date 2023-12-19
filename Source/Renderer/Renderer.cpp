#include "Renderer.h"
#include "RendererErrors.h"
#include "RenderTarget.h"
#include "SceneGraph/Camera.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::Renderer;

Renderer::Renderer(const std::string& name) : IRenderer(name)
{
    m_renderPacksArray[static_cast<size_t>(RenderListID::Overlay)].EnableSortBeforeDraw(false);
    m_renderPacksArray[static_cast<size_t>(RenderListID::DeferredLighting)].EnableSortBeforeDraw(false);
    m_renderPacksArray[static_cast<size_t>(RenderListID::OffSurface)].SortByDistance();
    m_stampBitMask = 0;
}

Renderer::~Renderer()
{
    FlushAll();
}

error Renderer::SetRenderTarget(const std::shared_ptr<RenderTarget>& target)
{
    m_target = target;
    return ErrorCode::ok;
}

std::shared_ptr<RenderTarget> Renderer::GetRenderTarget() const
{
    if (!m_target.expired()) return m_target.lock();
    return nullptr;
}

error Renderer::ChangeClearingProperty(const RenderTargetClearChangingProperty& prop)
{
    if (!GetRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = GetRenderTarget()->ChangeClearingProperty(prop);
    return er;
}

error Renderer::ClearRenderTarget()
{
    if (!GetRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = GetRenderTarget()->Clear();
    return er;
}

error Renderer::Flip()
{
    if (!GetRenderTarget()) return ErrorCode::nullRenderTarget;
    const error er = GetRenderTarget()->Flip();
    return er;
}

error Renderer::InsertRenderElement(const std::shared_ptr<RenderElement>& element,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lighting, RenderListID list_id)
{
    assert(element);
    error er;
    if ((list_id == RenderListID::OffSurface) && (!m_associatedCamera.expired()))
    {
        er = m_renderPacksArray[static_cast<size_t>(list_id)].
            InsertRenderElement(element, mxWorld, m_associatedCamera.lock()->location(), lighting, m_stampBitMask);
    }
    else
    {
        er = m_renderPacksArray[static_cast<size_t>(list_id)].
            InsertRenderElement(element, mxWorld, lighting, m_stampBitMask);
    }
    return er;
}

error Renderer::RemoveRenderElement(const std::shared_ptr<RenderElement>& element, RenderListID list_id)
{
    return m_renderPacksArray[static_cast<size_t>(list_id)].RemoveRenderElement(element, m_stampBitMask);
}

error Renderer::BeginScene()
{
    if (!m_target.expired())
    {
        m_target.lock()->Bind();
        m_target.lock()->Clear();
        m_target.lock()->BindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_target.lock()->GetViewPort());
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
    Graphics::IGraphicAPI::instance()->BeginScene();
    return ErrorCode::ok;
}

error Renderer::BeginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj)
{
    if (!m_target.expired())
    {
        m_target.lock()->Bind();
        m_target.lock()->Clear();
        m_target.lock()->BindViewPort();
        Engine::MaterialVariableMap::useViewPortDimension(m_target.lock()->GetViewPort());
    }
    Engine::MaterialVariableMap::useCameraParameter(camera_loc, mxView, mxProj);
    Graphics::IGraphicAPI::instance()->BeginScene();
    return ErrorCode::ok;
}

error Renderer::PrepareScene(const SceneGraph::VisibleSet& visible_set,
    SceneGraph::Spatial::SpatialFlags accept_filter, SceneGraph::Spatial::SpatialFlags reject_filter)
{
    const size_t count = visible_set.GetCount();
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

error Renderer::DrawScene()
{
    for (size_t i = 0; i < m_renderPacksArray.size(); i++)
    {
        if (!m_renderPacksArray[i].HasElements()) continue;

        if (const error er = m_renderPacksArray[i].Draw(m_stampBitMask, m_rendererTechniqueName)) return er;
        // ui 的 render 跟順序有關，除了不做排序外，element的cache也有可能會破壞順序，所以要多花時間做flush
        if (i == static_cast<size_t>(RenderListID::Overlay)) m_renderPacksArray[i].FlushAll(m_stampBitMask);
    }

    return ErrorCode::ok;
}

error Renderer::EndScene()
{
    Graphics::IGraphicAPI::instance()->EndScene();
    return ErrorCode::ok;
}

void Renderer::FlushAll()
{
    for (auto& pack : m_renderPacksArray)
    {
        pack.FlushAll(m_stampBitMask);
    }
}

void Renderer::SetAssociatedCamera(const std::shared_ptr<SceneGraph::Camera>& camera)
{
    m_associatedCamera = camera;
}

void Renderer::EnableSortBeforeDraw(RenderListID list_id, bool flag)
{
    if (static_cast<size_t>(list_id) >= static_cast<size_t>(RenderListID::Count)) return;
    m_renderPacksArray[static_cast<size_t>(list_id)].EnableSortBeforeDraw(flag);
}

void Renderer::selectRendererTechnique(const std::string& techniqueName)
{
    m_rendererTechniqueName = techniqueName;
}
