#include "CascadeShadowMapRenderer.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/RendererErrors.h"
#include "CSMSunLightCamera.h"
#include "GameEngine/MaterialVariableMap.h"
#include "GraphicKernel/IGraphicAPI.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

CascadeShadowMapRenderer::CascadeShadowMapRenderer(const std::string& name) : Renderer(name)
{
}

CascadeShadowMapRenderer::~CascadeShadowMapRenderer()
{
}

error CascadeShadowMapRenderer::DrawScene()
{
    if (LOG_IF(Warnning, ((m_targetViewPorts.empty()) || (m_sunLightCamera.expired())))) return ErrorCode::ok;
    unsigned pipeline_count = static_cast<unsigned>(m_targetViewPorts.size());
    if (m_sunLightCamera.lock()->GetPartitionCount() < pipeline_count) pipeline_count = m_sunLightCamera.lock()->GetPartitionCount();

    for (unsigned int pipeline = 0; pipeline < pipeline_count; pipeline++)
    {
        Graphics::IGraphicAPI::instance()->Bind(m_targetViewPorts[pipeline]);
        MaterialVariableMap::UseViewPortDimension(
            m_targetViewPorts[pipeline].Width(), m_targetViewPorts[pipeline].Height(),
            m_targetViewPorts[pipeline].MinZ(), m_targetViewPorts[pipeline].MaxZ());
        MaterialVariableMap::UseCameraParameter(m_sunLightCamera.lock()->GetLightCameraLocation(pipeline),
                                                        m_sunLightCamera.lock()->GetLightViewTransform(pipeline),
                                                        m_sunLightCamera.lock()->GetLightProjectionTransform(pipeline));
        if (pipeline == 0) // first, remove dated element
        {
            for (unsigned int i = 0; i < m_renderPacksArray.size(); i++)
            {
                if (!m_renderPacksArray[i].HasElements()) continue;
                error er = m_renderPacksArray[i].DrawWithRemoveDated(m_stampBitMask, m_rendererTechniqueName);
                if (er) return er;
            }
            //! render buffer draw 會清除 effect pass, 不用了 GraphicBinder()->EffectCleanupCache();
        }
        else if (pipeline == pipeline_count - 1) // last, mark element dated
        {
            for (unsigned int i = 0; i < m_renderPacksArray.size(); i++)
            {
                if (!m_renderPacksArray[i].HasElements()) continue;
                error er = m_renderPacksArray[i].DrawWithMarkDated(m_stampBitMask, m_rendererTechniqueName);
                if (er) return er;
            }
            //GraphicBinder()->EffectCleanupCache();
        }
        else
        {
            for (unsigned int i = 0; i < m_renderPacksArray.size(); i++)
            {
                if (!m_renderPacksArray[i].HasElements()) continue;
                error er = m_renderPacksArray[i].DrawOnlyNative(m_stampBitMask, m_rendererTechniqueName);
                if (er) return er;
            }
            //GraphicBinder()->EffectCleanupCache();
        }
    }

    return ErrorCode::ok;
}

void CascadeShadowMapRenderer::SetRenderTargetViewPorts(const std::vector<Graphics::TargetViewPort>& view_ports)
{
    m_targetViewPorts = view_ports;
}

void CascadeShadowMapRenderer::SetSunLightCamera(const std::shared_ptr<CSMSunLightCamera>& sun_light_camera)
{
    m_sunLightCamera = sun_light_camera;
}
