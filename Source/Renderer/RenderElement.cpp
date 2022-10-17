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
    Engine::EffectMaterialPtr& effect, const Engine::GeometrySegment& segment)
{
    m_rendererStamp = 0;
    m_rendererActiveFrameFlag = 0;
    m_renderBuffer = renderBuffer;
    m_effectMaterial = std::move(effect);
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

error RenderElement::Draw(const MathLib::Matrix4& mxWorld,
    const Engine::RenderLightingState& state, const std::string& rendererTechnique)
{
    if (m_renderBuffer.expired()) return ErrorCode::nullRenderBuffer;
    if (!m_effectMaterial) return ErrorCode::nullEffectMaterial;
    m_effectMaterial->SelectRendererTechnique(rendererTechnique);
    state.CommitState();
    Engine::MaterialVariableMap::UseWorldTransform(mxWorld);
    error er = m_renderBuffer.lock()->Draw(m_effectMaterial, m_segment);
    return er;
}

error RenderElement::DrawExternal(const MathLib::Matrix4& mxWorld, const Engine::EffectMaterialPtr& effect)
{
    if (m_renderBuffer.expired()) return ErrorCode::nullRenderBuffer;
    if (!effect) return ErrorCode::nullEffectMaterial;
    Engine::MaterialVariableMap::UseWorldTransform(mxWorld);
    error er = m_renderBuffer.lock()->Draw(effect, m_segment);
    return er;
}
