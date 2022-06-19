#include "RenderTarget.h"
#include "EngineErrors.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/GraphicThread.h"
#include <cassert>

using namespace Enigma::Engine;

RenderTarget::RenderTarget(const std::string& name, PrimaryType primary, Graphics::IGraphicAPI::AsyncType async)
{
    m_isPrimary = primary == PrimaryType::IsPrimary;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_backSurface = nullptr;
    m_depthStencilSurface = nullptr;

    m_gbufferDepthMapIndex = 0;

    if (m_isPrimary)
    {
        if (async == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
        {
            Graphics::IGraphicAPI::Instance()->AsyncGetPrimaryBackSurface(&m_backSurface, &m_depthStencilSurface).wait();
            assert(m_backSurface);
        }
        else
        {
            Graphics::IGraphicAPI::Instance()->GetPrimaryBackSurface(&m_backSurface, &m_depthStencilSurface);
            assert(m_backSurface);
        }
    }
}

RenderTarget::RenderTarget(const std::string& name, const Graphics::IBackSurfacePtr& back_surface, 
    Graphics::IDepthStencilSurfacePtr& depth_surface, bool primary)
{
    m_isPrimary = primary;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_backSurface = back_surface;
    m_depthStencilSurface = depth_surface;

    m_gbufferDepthMapIndex = 0;
}

RenderTarget::~RenderTarget()
{
    m_renderTargetTexture = nullptr;

    m_backSurface = nullptr;
    m_depthStencilSurface = nullptr;
}

error RenderTarget::Initialize()
{
    if ((m_backSurface) && (m_depthStencilSurface))
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
    CreateRenderTargetTexture();
    InitViewPortSize();
    return ErrorCode::ok;
}

future_error Enigma::Engine::RenderTarget::AsyncInitialize()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Initialize(); });
}
