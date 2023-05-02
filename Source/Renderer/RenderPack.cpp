#include "RenderPack.h"

using namespace Enigma::Renderer;
using namespace Enigma::MathLib;

RenderPack::RenderPack()
{
    m_worldTransform = Matrix4::IDENTITY;
    m_element = nullptr;
    m_squareCameraDistance = 0.0f;
}

RenderPack::RenderPack(const std::shared_ptr<RenderElement>& element,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lighting_state)
{
    m_element = element;
    m_worldTransform = mxWorld;
    m_renderLightingState = lighting_state;
    m_squareCameraDistance = 0.0f;
}

RenderPack::RenderPack(const RenderPack& pack)
{
    m_element = pack.m_element;
    m_worldTransform = pack.m_worldTransform;
    m_renderLightingState = pack.m_renderLightingState;
    m_squareCameraDistance = pack.m_squareCameraDistance;
}

RenderPack::RenderPack(RenderPack&& pack) noexcept
{
    m_element = std::move(pack.m_element);
    m_worldTransform = pack.m_worldTransform;
    m_renderLightingState = std::move(pack.m_renderLightingState);
    m_squareCameraDistance = pack.m_squareCameraDistance;
}

RenderPack::~RenderPack()
{
    m_element = nullptr;
}

RenderPack& RenderPack::operator=(const RenderPack& pack)
{
    if (this == &pack) return *this;
    m_element = pack.m_element;
    m_worldTransform = pack.m_worldTransform;
    m_renderLightingState = pack.m_renderLightingState;
    m_squareCameraDistance = pack.m_squareCameraDistance;

    return *this;
}

RenderPack& RenderPack::operator=(RenderPack&& pack) noexcept
{
    m_element = std::move(pack.m_element);
    m_worldTransform = pack.m_worldTransform;
    m_renderLightingState = std::move(pack.m_renderLightingState);
    m_squareCameraDistance = pack.m_squareCameraDistance;

    return *this;
}

bool RenderPack::operator==(const RenderPack& p) const
{
    return m_element == p.m_element;
}

bool RenderPack::operator!=(const RenderPack& p) const
{
    return m_element != p.m_element;
}

const std::shared_ptr<RenderElement>& RenderPack::GetRenderElement()
{
    return m_element;
}

const std::shared_ptr<RenderElement>& RenderPack::GetRenderElement() const
{
    return m_element;
}

Matrix4 RenderPack::GetWorldTransform()
{
    return m_worldTransform;
}

const Matrix4& RenderPack::GetWorldTransform() const
{
    return m_worldTransform;
}

const Enigma::Engine::RenderLightingState& RenderPack::GetRenderLightingState() const
{
    return m_renderLightingState;
}

void RenderPack::SetWorldTransform(const MathLib::Matrix4& mxWorld)
{
    m_worldTransform = mxWorld;
}

void RenderPack::SetRenderLightingState(const Engine::RenderLightingState& state)
{
    m_renderLightingState = state;
}

void RenderPack::CalcSquareCameraDistance(const MathLib::Vector3& camera_loc)
{
    const Vector3 dist = m_worldTransform.UnMatrixTranslate() - camera_loc;
    m_squareCameraDistance = dist.SquaredLength();
}
