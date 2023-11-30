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
    SubscribeEvents();
    Engine::MaterialVariableMap::InsertAutoVariableFunctionToMap(m_configuration->ShadowMapDimensionSemantic(), AssignShadowMapDimension);
    Engine::MaterialVariableMap::InsertAutoVariableFunctionToMap(m_configuration->LightViewProjSemantic(), AssignLightViewProjectionTransforms);
    Engine::MaterialVariableMap::InsertAutoVariableFunctionToMap(m_configuration->CascadeDistanceSemantic(), AssignCascadeDistances);
    Engine::MaterialVariableMap::InsertAutoVariableFunctionToMap(m_configuration->CascadeTextureCoordTransformSemantic(), AssignCascadeTextureCoordTransforms);
    Engine::MaterialVariableMap::InsertAutoVariableFunctionToMap(m_configuration->CascadeSliceCountSemantic(), AssignSliceCount);
    return ServiceResult::Complete;
}

ServiceResult CascadeShadowMapService::onTick()
{
    if ((!m_sceneService.expired()) && (m_sunLightCamera))
    {
        m_sunLightCamera->CalcLightCameraSystemMatrix(m_sceneService.lock()->GetSceneCuller());
        m_cascadeLightViewProjections = m_sunLightCamera->GetLightViewProjectionTransforms();
        m_cascadeDistances = m_sunLightCamera->LightFrustaDistanceToVector4();
        m_cascadeTextureCoordTransforms = m_sunLightCamera->GetTextureCoordTransforms();
    }
    return ServiceResult::Pendding;
}

ServiceResult CascadeShadowMapService::onTerm()
{
    m_sunLightCamera = nullptr;
    UnsubscribeEvents();
    return ServiceResult::Complete;
}

void CascadeShadowMapService::CreateShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    std::vector<TargetViewPort> viewPorts(m_configuration->FrustaPartitionCount());
    for (unsigned int i = 0; i < m_configuration->FrustaPartitionCount(); i++)
    {
        viewPorts[i].Width() = m_configuration->ShadowMapDimension().m_width;
        viewPorts[i].Height() = m_configuration->ShadowMapDimension().m_height;
        viewPorts[i].X() = i * m_configuration->ShadowMapDimension().m_width;
        viewPorts[i].Y() = 0;
    }
    m_shadowMapDimensionBiasDensity[0] = static_cast<float>(m_configuration->ShadowMapDimension().m_width);
    m_shadowMapDimensionBiasDensity[1] = static_cast<float>(m_configuration->ShadowMapDimension().m_height);
    m_shadowMapDimensionBiasDensity[2] = m_configuration->ShadowMapDepthBias();
    m_shadowMapDimensionBiasDensity[3] = m_configuration->ShadowMapDensity();

    MathLib::Dimension<unsigned> fullDimension{ m_configuration->ShadowMapDimension().m_width * m_configuration->FrustaPartitionCount(),
        m_configuration->ShadowMapDimension().m_height };
    TargetViewPort fullViewPort(0, 0, fullDimension.m_width, fullDimension.m_height);
    Engine::IRendererPtr renderer = std::make_shared<CascadeShadowMapRenderer>(renderer_name);
    error er = m_rendererManager.lock()->InsertRenderer(renderer_name, renderer);
    if (er) return;
    m_rendererManager.lock()->CreateRenderTarget(target_name, RenderTarget::PrimaryType::NotPrimary, { Graphics::RenderTextureUsage::ShadowMap });

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(m_rendererManager.lock()->GetRenderer(renderer_name));
    m_renderer.lock()->SelectRendererTechnique(m_configuration->ShadowMapTechniqueName());
    auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock());
    rendererCSM->SetRenderTargetViewPorts(viewPorts);

    m_shadowMapRenderTarget = m_rendererManager.lock()->GetRenderTarget(target_name);
    m_shadowMapRenderTarget.lock()->InitBackSurface(m_configuration->ShadowMapSurfaceName(), fullDimension, Graphics::GraphicFormat::FMT_R32F);
    m_shadowMapRenderTarget.lock()->InitDepthStencilSurface(m_configuration->ShadowMapDepthName(), fullDimension,
        Graphics::IGraphicAPI::instance()->GetDepthSurfaceFormat());
    m_renderer.lock()->SetRenderTarget(m_shadowMapRenderTarget.lock());
    m_shadowMapRenderTarget.lock()->ChangeClearingProperty(RenderTargetClearChangingProperty{ MathLib::ColorRGBA(1.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0, std::nullopt });
    m_shadowMapRenderTarget.lock()->SetViewPort(fullViewPort);

    if (m_sunLightCamera)
    {
        rendererCSM->SetSunLightCamera(m_sunLightCamera);
    }
}

void CascadeShadowMapService::DestroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    m_rendererManager.lock()->DestroyRenderTarget(target_name);
    m_rendererManager.lock()->DestroyRenderer(renderer_name);
}

void CascadeShadowMapService::CreateSunLightCamera(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(!m_cameraService.expired());
    m_sunLightCamera = std::make_shared<CSMSunLightCamera>(SceneGraph::SpatialId(m_configuration->SunLightCameraName(), CSMSunLightCamera::TYPE_RTTI), m_configuration->FrustaPartitionCount());
    MathLib::Vector3 vecSunDir = MathLib::Vector3(-1.0f, -1.0f, 0.0f);
    if (lit) vecSunDir = lit->GetLightDirection();
    m_sunLightCamera->SetSunLightDir(vecSunDir);
    if (auto cam = m_cameraService.lock()->primaryCamera())
    {
        m_sunLightCamera->SetViewerCamera(cam);
    }
    if (!m_renderer.expired())
    {
        m_renderer.lock()->SetAssociatedCamera(m_sunLightCamera);
        if (const auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock()))
        {
            rendererCSM->SetSunLightCamera(m_sunLightCamera);
        }
    }
}

void CascadeShadowMapService::DeleteSunLightCamera()
{
    if (!m_renderer.expired())
    {
        m_renderer.lock()->SetAssociatedCamera(nullptr);
        if (const auto rendererCSM = std::dynamic_pointer_cast<CascadeShadowMapRenderer, Renderer::Renderer>(m_renderer.lock()))
        {
            rendererCSM->SetSunLightCamera(nullptr);
        }
    }
    m_sunLightCamera = nullptr;
}

void CascadeShadowMapService::UpdateSunLightDirection(const MathLib::Vector3& dir)
{
    if (m_sunLightCamera)
    {
        m_sunLightCamera->SetSunLightDir(dir);
    }
}

void CascadeShadowMapService::AssignLightViewProjectionTransforms(Engine::EffectVariable& var)
{
    var.AssignValues(m_cascadeLightViewProjections, static_cast<unsigned>(m_cascadeLightViewProjections.size()));
}

void CascadeShadowMapService::AssignCascadeDistances(Engine::EffectVariable& var)
{
    var.AssignValue(m_cascadeDistances);
}

void CascadeShadowMapService::AssignCascadeTextureCoordTransforms(Engine::EffectVariable& var)
{
    var.AssignValues(m_cascadeTextureCoordTransforms, static_cast<unsigned>(m_cascadeTextureCoordTransforms.size()));
}

void CascadeShadowMapService::AssignSliceCount(Engine::EffectVariable& var)
{
    var.AssignValue(static_cast<int>(m_cascadeLightViewProjections.size()));
}

void CascadeShadowMapService::AssignSliceDimension(Engine::EffectVariable& var)
{

}

void CascadeShadowMapService::AssignFaceLightThreshold(Engine::EffectVariable& var)
{

}
