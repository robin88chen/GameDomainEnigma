#include "SceneRendererService.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;

DEFINE_RTTI(GameCommon, SceneRendererService, ISystemService);

SceneRendererService::SceneRendererService(ServiceManager* mngr, const std::shared_ptr<GameSceneService>& scene_service,
    const std::shared_ptr<Renderer::RendererManager>& renderer_manager, const std::string& renderer_name) : ISystemService(mngr)
{
    m_sceneService = scene_service;
    m_rendererManager = renderer_manager;
    m_rendererName = renderer_name;
    m_needTick = false;
}

SceneRendererService::~SceneRendererService()
{

}

ServiceResult SceneRendererService::OnInit()
{
    assert(!m_rendererManager.expired());
    m_renderer = m_rendererManager.lock()->GetRenderer(m_rendererName);
    return ServiceResult::Complete;
}

ServiceResult SceneRendererService::OnTerm()
{
    return ServiceResult::Complete;
}
