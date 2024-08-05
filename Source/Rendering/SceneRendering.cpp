#include "SceneRendering.h"
#include "SceneRenderingConfiguration.h"
#include "Renderer/RendererManager.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererEvents.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include <cassert>
#include <system_error>

#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Rendering;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(Rendering, SceneRendering, ISystemService);

SceneRendering::SceneRendering(ServiceManager* mngr, const std::shared_ptr<RendererManager>& renderer_manager, const std::shared_ptr<IRenderingConfiguration>& config) : ISystemService(mngr)
{
    m_config = std::dynamic_pointer_cast<SceneRenderingConfiguration>(config);
    assert(m_config);
    m_rendererManager = renderer_manager;
    m_needTick = false;
}

SceneRendering::~SceneRendering()
{

}

ServiceResult SceneRendering::onInit()
{
    m_onCameraCreated = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onCameraCreatedOrConstituted(e); });
    m_onCameraConstituted = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onCameraCreatedOrConstituted(e); });
    m_onPrimaryTargetCreated = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onPrimaryTargetCreated(e); });
    EventPublisher::subscribe(typeid(CameraCreated), m_onCameraCreated);
    EventPublisher::subscribe(typeid(CameraConstituted), m_onCameraConstituted);
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryTargetCreated);

    return ServiceResult::Complete;
}

ServiceResult SceneRendering::onTerm()
{
    EventPublisher::unsubscribe(typeid(CameraCreated), m_onCameraCreated);
    EventPublisher::unsubscribe(typeid(CameraConstituted), m_onCameraConstituted);
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryTargetCreated);
    m_onCameraCreated = nullptr;
    m_onCameraConstituted = nullptr;
    m_onPrimaryTargetCreated = nullptr;

    return ServiceResult::Complete;
}

void SceneRendering::createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(m_config);
    assert(!m_rendererManager.expired());
    error er = m_rendererManager.lock()->createRenderer(renderer_name);
    if (er) return;
    er = m_rendererManager.lock()->createRenderTarget(target_name, m_config->isPrimary() ? RenderTarget::PrimaryType::IsPrimary : RenderTarget::PrimaryType::NotPrimary, { Graphics::RenderTextureUsage::Default });
    if (er) return;
    auto renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(m_rendererManager.lock()->getRenderer(renderer_name));
    renderer->selectRendererTechnique(m_config->primaryRendererTechniqueName());
    auto target = m_rendererManager.lock()->getRenderTarget(target_name);
    std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(renderer)->setRenderTarget(target);
    m_renderer = renderer;
    attachCamera();
}

void SceneRendering::destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->destroyRenderTarget(target_name);
    m_rendererManager.lock()->destroyRenderer(renderer_name);
}

void SceneRendering::prepareGameScene(const SceneGraph::VisibleSet& visible_set)
{
    if ((!m_renderer.expired()) && (visible_set.getCount() != 0))
    {
        m_renderer.lock()->prepareScene(visible_set);
    }
}

void SceneRendering::renderGameScene()
{
    if (m_renderer.expired()) return;
    m_renderer.lock()->clearRenderTarget();
    m_renderer.lock()->beginScene();
    m_renderer.lock()->drawScene();
    m_renderer.lock()->endScene();
}

void SceneRendering::flip()
{
    if (m_renderer.expired()) return;
    m_renderer.lock()->flip();
}

void SceneRendering::attachCamera()
{
    assert(m_config);
    if (m_renderer.expired()) return;
    std::shared_ptr<Camera> camera = std::make_shared<QueryRunningCamera>(m_config->primaryCameraId())->dispatch();
    if (!camera) return;
    m_renderer.lock()->setAssociatedCamera(camera);
    auto [w, h] = m_renderer.lock()->getRenderTarget()->getDimension();
    if ((w > 0) && (h > 0))
    {
        camera->changeAspectRatio(static_cast<float>(w) / static_cast<float>(h));
    }
}

void SceneRendering::onCameraCreatedOrConstituted(const IEventPtr& e)
{
    if (!e) return;
    SpatialId camera_id;
    if (const auto camera_created = std::dynamic_pointer_cast<CameraCreated>(e))
    {
        camera_id = camera_created->camera()->id();
    }
    else if (const auto camera_constituted = std::dynamic_pointer_cast<CameraConstituted>(e))
    {
        camera_id = camera_constituted->camera()->id();
    }
    if (camera_id != m_config->primaryCameraId()) return;
    attachCamera();
}

void SceneRendering::onPrimaryTargetCreated(const Frameworks::IEventPtr& e)
{
    attachCamera();
}

