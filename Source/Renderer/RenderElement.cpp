#include "RenderElement.h"
#include "RendererErrors.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/RenderLightingState.h"
#include "GameEngine/MaterialVariableMap.h"

using namespace Enigma::Renderer;

RenderElement::RenderElement()
{
    m_rendererStamp = 0;
    m_rendererActiveFrameFlag = 0;
    m_effectMaterial = nullptr;
}

RenderElement::RenderElement(const std::shared_ptr<Engine::RenderBuffer>& renderBuffer,
    const std::shared_ptr<Engine::EffectMaterial>& effect, const Geometries::GeometrySegment& segment)
{
    m_rendererStamp = 0;
    m_rendererActiveFrameFlag = 0;
    m_renderBuffer = renderBuffer;
    m_effectMaterial = effect;
    m_segment = segment;
}

RenderElement::~RenderElement()
{
    m_effectMaterial = nullptr;
}

std::shared_ptr<Enigma::Engine::RenderBuffer> RenderElement::GetRenderBuffer() const
{
    if (!m_renderBuffer.expired()) return m_renderBuffer.lock();
    return nullptr;
}

error RenderElement::draw(const MathLib::Matrix4& mxWorld,
    const Engine::RenderLightingState& state, const std::string& rendererTechnique)
{
    if (m_renderBuffer.expired()) return ErrorCode::nullRenderBuffer;
    if (!m_effectMaterial) return ErrorCode::nullEffectMaterial;
    m_effectMaterial->selectRendererTechnique(rendererTechnique);
    state.commitState();
    Engine::MaterialVariableMap::useWorldTransform(mxWorld);
    const error er = m_renderBuffer.lock()->draw(m_effectMaterial, m_segment);
    return er;
}

error RenderElement::DrawExternal(const MathLib::Matrix4& mxWorld, const std::shared_ptr<Engine::EffectMaterial>& effect)
{
    if (m_renderBuffer.expired()) return ErrorCode::nullRenderBuffer;
    if (!effect) return ErrorCode::nullEffectMaterial;
    Engine::MaterialVariableMap::useWorldTransform(mxWorld);
    const error er = m_renderBuffer.lock()->draw(effect, m_segment);
    return er;
}
