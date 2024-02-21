#include "CascadeShadowMapService.h"
#include "CascadeShadowMapRenderer.h"
#include "CSMSunLightCamera.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraph/Light.h"
#include "GameCommon/GameCameraService.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::Frameworks;
using namespace Enigma::GameCommon;
using namespace Enigma::Renderer;
using namespace Enigma::Graphics;

DEFINE_RTTI(ShadowMap, CascadeShadowMapService, ShadowMapService);

CascadeShadowMapService::CascadeShadowMapService(ServiceManager* manager,
    const std::shared_ptr<GameSceneService>& scene_service,
    const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<RendererManager>& renderer_manager,
    const std::shared_ptr<CascadeShadowMapServiceConfiguration>& configuration) : ShadowMapService(manager, scene_service, camera_service, renderer_manager, configuration)
{
    m_configuration = configuration;
}

CascadeShadowMapService::~CascadeShadowMapService()
{
    m_configuration = nullptr;
    m_sunLightCamera = nullptr;
}

ServiceResult CascadeShadowMapService::onInit()
{
    subscribeEvents();
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->shadowMapDimensionSemantic(), assignShadowMapDimension);
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->lightViewProjSemantic(), AssignLightViewProjectionTransforms);
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->CascadeDistanceSemantic(), AssignCascadeDistances);
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->CascadeTextureCoordTransformSemantic(), AssignCascadeTextureCoordTransforms);
    Engine::MaterialVariableMap::insertAutoVariableFunctionToMap(m_configuration->CascadeSliceCountSemantic(), AssignSliceCount);
    return ServiceResult::Complete;
}

ServiceResult CascadeShadowMapService::onTick()
{
    if ((!m_sceneService.expired()) && (m_sunLightCamera))
    {
        m_sunLightCamera->CalcLightCameraSystemMatrix(m_sceneService.lock()->getSceneCuller());
        m_cascadeLightViewProjections = m_sunLightCamera->GetLightViewProjectionTransforms();
        m_cascadeDistances = m_sunLightCamera->LightFrustaDistanceToVector4();
        m_cascadeTextureCoordTransforms = m_sunLightCamera->GetTextureCoordTransforms();
    }
    return ServiceResult::Pendding;
}

ServiceResult CascadeShadowMapService::onTerm()
{
    m_sunLightCamera = nullptr;
    unsubscribeEvents();
    return ServiceResult::Complete;
}

void CascadeShadowMapService::createShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    std::vector<TargetViewPort> viewPorts(m_configuration->FrustaPartitionCount());
    for (unsigned int i = 0; i < m_configuration->FrustaPartitionCount(); i++)
    {
        viewPorts[i].Width() = m_configuration->shadowMapDimension().m_width;
        viewPorts[i].Height() = m_configuration->shadowMapDimension().m_height;
        viewPorts[i].x() = i * m_configuration->shadowMapDimension().m_width;
        viewPorts[i].y() = 0;
    }
    m_shadowMapDimensionBiasDensity[0] = static_cast<float>(m_configuration->shadowMapDimension().m_width);
    m_shadowMapDimensionBiasDensity[1] = static_cast<float>(m_configuration->shadowMapDimension().m_height);
    m_shadowMapDimensionBiasDensity[2] = m_configuration->shadowMapDepthBias();
    m_shadowMapDimensionBiasDensity[3] = m_configuration->shadowMapDensity();

    MathLib::Dimension<unsigned> fullDimension{ m_configuration->shadowMapDimension().m_width * m_configuration->FrustaPartitionCount(),
        m_configuration->shadowMapDimension().m_height };
    TargetViewPort fullViewPort(0, 0, fullDimension.m_width, fullDimension.m_height);
    Engine::IRendererPtr renderer = std::make_shared<CascadeShadowMapRenderer>(renderer_name);
    error er = m_rendererManager.lock()->insertRenderer(renderer_name, renderer);
    if (er) return;
    m_rendererManager.lock()->createRenderTarget(target_name, Graphics::BackSurfaceSpecification(m_configuration->shadowMapSurfaceName(), fullDimension, Graphics::GraphicFormat::FMT_R32F), Graphics::DepthStencilSurfaceSpecification(m_configuration->shadowMapDepthName(), fullDimension, Graphics::IGraphicAPI::instance()->getDepthSurfaceFormat()), { Graphics::RenderTextureUsage::ShadowMap });

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(m_rendererManager.lock()->getRenderer(renderer_name));
    m_renderer.lock()->selectRendererTechnique(m_configuration->shadowMapTechniqueName());
    auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock());
    rendererCSM->SetRenderTargetViewPorts(viewPorts);

    m_shadowMapRenderTarget = m_rendererManager.lock()->getRenderTarget(target_name);
    //m_shadowMapRenderTarget.lock()->initBackSurface(Graphics::BackSurfaceSpecification(m_configuration->shadowMapSurfaceName(), fullDimension, Graphics::GraphicFormat::FMT_R32F));
    //m_shadowMapRenderTarget.lock()->initDepthStencilSurface(Graphics::DepthStencilSurfaceSpecification(m_configuration->shadowMapDepthName(), fullDimension,        Graphics::IGraphicAPI::instance()->getDepthSurfaceFormat()));
    m_renderer.lock()->setRenderTarget(m_shadowMapRenderTarget.lock());
    m_shadowMapRenderTarget.lock()->changeClearingProperty(RenderTargetClearChangingProperty{ MathLib::ColorRGBA(1.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0, std::nullopt });
    m_shadowMapRenderTarget.lock()->setViewPort(fullViewPort);

    if (m_sunLightCamera)
    {
        rendererCSM->SetSunLightCamera(m_sunLightCamera);
    }
}

void CascadeShadowMapService::destroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->destroyRenderTarget(target_name);
    m_rendererManager.lock()->destroyRenderer(renderer_name);
}

void CascadeShadowMapService::createSunLightCamera(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(!m_cameraService.expired());
    m_sunLightCamera = std::make_shared<CSMSunLightCamera>(m_configuration->sunLightCameraId(), m_configuration->FrustaPartitionCount());
    MathLib::Vector3 vecSunDir = MathLib::Vector3(-1.0f, -1.0f, 0.0f);
    if (lit) vecSunDir = lit->getLightDirection();
    m_sunLightCamera->setSunLightDir(vecSunDir);
    if (auto cam = m_cameraService.lock()->primaryCamera())
    {
        m_sunLightCamera->setViewerCamera(cam);
    }
    if (!m_renderer.expired())
    {
        m_renderer.lock()->setAssociatedCamera(m_sunLightCamera);
        if (const auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock()))
        {
            rendererCSM->SetSunLightCamera(m_sunLightCamera);
        }
    }
}

void CascadeShadowMapService::deleteSunLightCamera()
{
    if (!m_renderer.expired())
    {
        m_renderer.lock()->setAssociatedCamera(nullptr);
        if (const auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock()))
        {
            rendererCSM->SetSunLightCamera(nullptr);
        }
    }
    m_sunLightCamera = nullptr;
}

void CascadeShadowMapService::updateSunLightDirection(const MathLib::Vector3& dir)
{
    if (m_sunLightCamera)
    {
        m_sunLightCamera->setSunLightDir(dir);
    }
}

void CascadeShadowMapService::AssignLightViewProjectionTransforms(Engine::EffectVariable& var)
{
    var.assignValues(m_cascadeLightViewProjections, static_cast<unsigned>(m_cascadeLightViewProjections.size()));
}

void CascadeShadowMapService::AssignCascadeDistances(Engine::EffectVariable& var)
{
    var.assignValue(m_cascadeDistances);
}

void CascadeShadowMapService::AssignCascadeTextureCoordTransforms(Engine::EffectVariable& var)
{
    var.assignValues(m_cascadeTextureCoordTransforms, static_cast<unsigned>(m_cascadeTextureCoordTransforms.size()));
}

void CascadeShadowMapService::AssignSliceCount(Engine::EffectVariable& var)
{
    var.assignValue(static_cast<int>(m_cascadeLightViewProjections.size()));
}

void CascadeShadowMapService::AssignSliceDimension(Engine::EffectVariable& var)
{

}

void CascadeShadowMapService::AssignFaceLightThreshold(Engine::EffectVariable& var)
{

}
