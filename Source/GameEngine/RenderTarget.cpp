#include "RenderTarget.h"
#include "EngineErrors.h"
#include "RendererEvents.h"
#include "RendererCommands.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/IBackSurface.h"
#include "GraphicKernel/GraphicThread.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Engine;

RenderTarget::RenderTarget(const std::string& name, PrimaryType primary, Graphics::IGraphicAPI::AsyncType async)
{
    m_isPrimary = primary == PrimaryType::IsPrimary;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_backSurface = nullptr;
    m_depthStencilSurface = nullptr;

    m_gbufferDepthMapIndex = 0;
    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, BufferClearFlag::BothBuffer };

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
        m_dimension = m_backSurface->GetDimension();
    }

    m_handleChangingViewPort = 
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->HandleChangingViewPort(c); });
    Frameworks::CommandBus::Subscribe(typeid(ChangeTargetViewPort), m_handleChangingViewPort);
    m_handleChangingClearingProperty =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->HandleChangingClearingProperty(c); });
    Frameworks::CommandBus::Subscribe(typeid(ChangeTargetClearingProperty), m_handleChangingClearingProperty);
}

RenderTarget::RenderTarget(const std::string& name, const Graphics::IBackSurfacePtr& back_surface, 
    Graphics::IDepthStencilSurfacePtr& depth_surface, bool primary)
{
    m_isPrimary = primary;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_backSurface = back_surface;
    m_depthStencilSurface = depth_surface;

    m_dimension = m_backSurface->GetDimension();

    m_gbufferDepthMapIndex = 0;
    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, BufferClearFlag::BothBuffer };
}

RenderTarget::~RenderTarget()
{
    Frameworks::CommandBus::Unsubscribe(typeid(ChangeTargetViewPort), m_handleChangingViewPort);
    m_handleChangingViewPort = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(ChangeTargetClearingProperty), m_handleChangingClearingProperty);
    m_handleChangingClearingProperty = nullptr;

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
    m_dimension = m_backSurface->GetDimension();
    CreateRenderTargetTexture();
    InitViewPortSize();
    return ErrorCode::ok;
}

future_error Enigma::Engine::RenderTarget::AsyncInitialize()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Initialize(); });
}

error RenderTarget::InitBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    error er = Graphics::IGraphicAPI::Instance()->CreateBackSurface(dimension, fmt, &m_backSurface);
    if (er) return er;
    m_dimension = m_backSurface->GetDimension();
    CreateRenderTargetTexture();
    InitViewPortSize();

    return er;
}

future_error RenderTarget::AsyncInitBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitBackSurface(dimension, fmt); });
}

error RenderTarget::InitMultiBackSurface(const MathLib::Dimension& dimension, unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    error er = Graphics::IGraphicAPI::Instance()->CreateBackSurface(dimension, surface_count, fmts, &m_backSurface);
    if (er) return er;
    m_dimension = m_backSurface->GetDimension();
    CreateRenderTargetTexture();
    InitViewPortSize();

    return er;
}

future_error RenderTarget::AsyncInitMultiBackSurface(const MathLib::Dimension& dimension, unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitMultiBackSurface(dimension, surface_count, fmts); });
}

error RenderTarget::InitDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    error er = Graphics::IGraphicAPI::Instance()->CreateDepthStencilSurface(dimension, fmt, &m_depthStencilSurface);
    if (er) return er;
    if (m_backSurface)
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
    return er;
}

future_error RenderTarget::AsyncInitDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitDepthStencilSurface(dimension, fmt); });
}

error RenderTarget::ShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& surface)
{
    error er = Graphics::IGraphicAPI::Instance()->ShareDepthStencilSurface(surface, &m_depthStencilSurface);
    if (er) return er;
    if (m_backSurface)
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
    return er;
}

future_error RenderTarget::AsyncShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& surface)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->ShareDepthStencilSurface(surface); });
}

const Enigma::Graphics::TargetViewPort& RenderTarget::GetViewPort()
{
    return m_viewPort;
}

error RenderTarget::Bind()
{
    assert(Graphics::IGraphicAPI::Instance());
    return Graphics::IGraphicAPI::Instance()->BindBackSurface(m_backSurface, m_depthStencilSurface);
}

future_error RenderTarget::AsyncBind()
{
    assert(Graphics::IGraphicAPI::Instance());
    assert(Graphics::IGraphicAPI::Instance()->GetGraphicThread());
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Bind(); });
}

error RenderTarget::BindViewPort()
{
    assert(Graphics::IGraphicAPI::Instance());
    return Graphics::IGraphicAPI::Instance()->BindViewPort(m_viewPort);
}

future_error RenderTarget::AsyncBindViewPort()
{
    assert(Graphics::IGraphicAPI::Instance());
    assert(Graphics::IGraphicAPI::Instance()->GetGraphicThread());
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->BindViewPort(); });
}

error RenderTarget::Clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value, BufferClearFlag flag)
{
    error er = Graphics::IGraphicAPI::Instance()->ClearSurface(
        ((int)flag & (int)BufferClearFlag::ColorBuffer) ? m_backSurface : nullptr,
        ((int)flag & (int)BufferClearFlag::DepthBuffer) ? m_depthStencilSurface : nullptr,
        color, depth_value, stencil_value);
    return er;
}

error RenderTarget::Clear()
{
    return Clear(m_clearingProperty.m_color, m_clearingProperty.m_depth, 
        m_clearingProperty.m_stencil, m_clearingProperty.m_flag);
}

future_error RenderTarget::AsyncClear()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Clear(); });
}

error RenderTarget::Flip()
{
    if (FATAL_LOG_EXPR(!m_isPrimary)) return ErrorCode::flipNotPrimary;

    return Graphics::IGraphicAPI::Instance()->Flip();
}

future_error RenderTarget::AsyncFlip()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Flip(); });
}

error RenderTarget::Resize(const MathLib::Dimension& dimension)
{
    if (FATAL_LOG_EXPR(!m_backSurface)) return ErrorCode::nullBackSurface;
    bool isCurrentBound = false;
    if (Graphics::IGraphicAPI::Instance()->CurrentBoundBackSurface() == m_backSurface)
    {
        isCurrentBound = true;
        Graphics::IGraphicAPI::Instance()->BindBackSurface(nullptr, nullptr);
    }
    error er = m_backSurface->Resize(dimension);
    if (er) return er;
    m_dimension = m_backSurface->GetDimension();

    // 好...來...因為back buffer surface實際上已經重新create了,
    // 所以這裡頭的shader resource view要重新綁定
    //todo : render target texture
    /*if (m_renderTargetTexture)
    {
        m_renderTargetTexture->GetDeviceTexture()->UseAsBackSurface(m_backSurface);
    }*/
    if (m_depthStencilSurface)
    {
        er = m_depthStencilSurface->Resize(dimension);
        if (er) return er;
    }
    if (isCurrentBound)
    {
        Graphics::IGraphicAPI::Instance()->BindBackSurface(m_backSurface, m_depthStencilSurface);
    }
    InitViewPortSize();

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew RenderTargetResized{ m_name, m_dimension } });

    return ErrorCode::ok;
}

future_error RenderTarget::AsyncResize(const MathLib::Dimension& dimension)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Resize(dimension); });
}

bool RenderTarget::HasGBufferDepthMap()
{
    if (!m_backSurface) return false;
    if (!m_backSurface->IsMultiSurface()) return false;
    if (m_gbufferDepthMapIndex == 0) return false;
    return true;
}

unsigned RenderTarget::GetGBufferDepthMapIndex()
{
    return m_gbufferDepthMapIndex;
}

void RenderTarget::SetGBufferDepthMapIndex(unsigned index)
{
    if (!m_backSurface) return;
    if (!m_backSurface->IsMultiSurface()) return;
    m_gbufferDepthMapIndex = index;
}

void RenderTarget::CreateRenderTargetTexture()
{
    //todo : render target texture
}

void RenderTarget::InitViewPortSize()
{
    m_viewPort.Width() = m_dimension.m_width;
    m_viewPort.Height() = m_dimension.m_height;

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew TargetViewPortInitialized{ m_name, m_viewPort } });
}

void RenderTarget::SetViewPort(const Graphics::TargetViewPort& vp)
{
    m_viewPort = vp;
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew TargetViewPortChanged{ m_name, m_viewPort } });
}

void RenderTarget::HandleChangingViewPort(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    ChangeTargetViewPort* cmd = dynamic_cast<ChangeTargetViewPort*>(c.get());
    if (!cmd) return;
    if (cmd->GetRenderTargetName() != m_name) return;
    SetViewPort(cmd->GetViewPort());
}

void RenderTarget::HandleChangingClearingProperty(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    ChangeTargetClearingProperty* cmd = dynamic_cast<ChangeTargetClearingProperty*>(c.get());
    if (!cmd) return;
    if (cmd->GetRenderTargetName() != m_name) return;
    if (cmd->GetClearingColor())
    {
        m_clearingProperty.m_color = cmd->GetClearingColor().value();
    }
    if (cmd->GetClearingDepth())
    {
        m_clearingProperty.m_depth = cmd->GetClearingDepth().value();
    }
    if (cmd->GetClearingStencil())
    {
        m_clearingProperty.m_stencil = cmd->GetClearingStencil().value();
    }
    if (cmd->GetClearingFlag())
    {
        m_clearingProperty.m_flag = cmd->GetClearingFlag().value();
    }
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{
        menew TargetClearingPropertyChanged{ m_name, m_clearingProperty } });
}
