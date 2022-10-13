#include "Renderer.h"
#include "RendererErrors.h"
#include "RenderTarget.h"

using namespace Enigma::Renderer;

Renderer::Renderer(const std::string& name)
{
    m_name = name;
}

Renderer::~Renderer()
{
    
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
    error er = GetRenderTarget()->ChangeClearingProperty(prop);
    return er;
}

error Renderer::ClearRenderTarget()
{
    if (!GetRenderTarget()) return ErrorCode::nullRenderTarget;
    error er = GetRenderTarget()->Clear();
    return er;
}

error Renderer::Flip()
{
    if (!GetRenderTarget()) return ErrorCode::nullRenderTarget;
    error er = GetRenderTarget()->Flip();
    return er;
}
