#include "RenderTarget.h"
#include "EngineErrors.h"
#include "RendererEvents.h"
#include "RendererCommands.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/IBackSurface.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Engine;

const std::string primary_back_surface_name = "primary_back_surface";
const std::string primary_depth_surface_name = "primary_depth_surface";

RenderTarget::RenderTarget(const std::string& name, PrimaryType primary, Graphics::IGraphicAPI::AsyncType async)
{
    m_async = async;
    m_isPrimary = primary == PrimaryType::IsPrimary;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_gbufferDepthMapIndex = 0;
    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, BufferClearFlag::BothBuffer };

    SubscribeHandler();

    if (m_isPrimary)
    {
        if (async == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
        {
            Graphics::IGraphicAPI::Instance()->AsyncCreatePrimaryBackSurface(primary_back_surface_name, primary_depth_surface_name);
        }
        else
        {
            Graphics::IGraphicAPI::Instance()->CreatePrimaryBackSurface(primary_back_surface_name, primary_depth_surface_name);
        }
    }

}

RenderTarget::RenderTarget(const std::string& name, Graphics::IGraphicAPI::AsyncType async)
{
    m_async = async;
    m_isPrimary = false;
    m_name = name;
    m_renderTargetTexture = nullptr;

    m_gbufferDepthMapIndex = 0;
    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, BufferClearFlag::BothBuffer };

    SubscribeHandler();
}

RenderTarget::~RenderTarget()
{
    UnsubscribeHandler();
    if (Graphics::IGraphicAPI::Instance())
    {
        if (m_isPrimary)
        {
            Graphics::IGraphicAPI::Instance()->RemoveGraphicAsset(primary_back_surface_name);
            Graphics::IGraphicAPI::Instance()->RemoveGraphicAsset(primary_depth_surface_name);
        }
        else
        {
            Graphics::IGraphicAPI::Instance()->RemoveGraphicAsset(m_backSurfaceName);
            Graphics::IGraphicAPI::Instance()->RemoveGraphicAsset(m_depthSurfaceName);
        }
    }
    m_renderTargetTexture = nullptr;
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

future_error RenderTarget::AsyncInitialize()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->Initialize(); });
}

error RenderTarget::InitBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    const Graphics::GraphicFormat& fmt)
{
    m_backSurfaceName = back_name;
    error er = Graphics::IGraphicAPI::Instance()->CreateBackSurface(back_name, dimension, fmt);

    return er;
}

future_error RenderTarget::AsyncInitBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitBackSurface(back_name, dimension, fmt); });
}

error RenderTarget::InitMultiBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    m_backSurfaceName = back_name;
    error er = Graphics::IGraphicAPI::Instance()->CreateBackSurface(back_name, dimension, surface_count, fmts);

    return er;
}

future_error RenderTarget::AsyncInitMultiBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitMultiBackSurface(
            back_name, dimension, surface_count, fmts); });
}

error RenderTarget::InitDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension, 
    const Graphics::GraphicFormat& fmt)
{
    m_depthSurfaceName = depth_name;
    error er = Graphics::IGraphicAPI::Instance()->CreateDepthStencilSurface(depth_name, dimension, fmt);
    return er;
}

future_error RenderTarget::AsyncInitDepthStencilSurface(const std::string& depth_name, 
    const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->InitDepthStencilSurface(depth_name, dimension, fmt); });
}

error RenderTarget::ShareDepthStencilSurface(const std::string& depth_name, 
    const Graphics::IDepthStencilSurfacePtr& surface)
{
    m_depthSurfaceName = depth_name;
    error er = Graphics::IGraphicAPI::Instance()->ShareDepthStencilSurface(depth_name, surface);
    return er;
}

future_error RenderTarget::AsyncShareDepthStencilSurface(const std::string& depth_name, 
    const Graphics::IDepthStencilSurfacePtr& surface)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()
        ->PushTask([=]() -> error { return shared_from_this()->ShareDepthStencilSurface(depth_name, surface); });
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

void RenderTarget::SubscribeHandler()
{
    m_onPrimarySurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnPrimarySurfaceCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::PrimarySurfaceCreated), m_onPrimarySurfaceCreated);
    m_onBackSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnBackSurfaceCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::BackSurfaceCreated), m_onBackSurfaceCreated);
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::MultiBackSurfaceCreated), m_onBackSurfaceCreated);
    m_onDepthSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnDepthSurfaceCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DepthSurfaceCreated), m_onDepthSurfaceCreated);
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DepthSurfaceShared), m_onDepthSurfaceCreated);

    m_handleChangingViewPort =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->HandleChangingViewPort(c); });
    Frameworks::CommandBus::Subscribe(typeid(ChangeTargetViewPort), m_handleChangingViewPort);
    m_handleChangingClearingProperty =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->HandleChangingClearingProperty(c); });
    Frameworks::CommandBus::Subscribe(typeid(ChangeTargetClearingProperty), m_handleChangingClearingProperty);
}

void RenderTarget::UnsubscribeHandler()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::PrimarySurfaceCreated), m_onPrimarySurfaceCreated);
    m_onPrimarySurfaceCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::BackSurfaceCreated), m_onBackSurfaceCreated);
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::MultiBackSurfaceCreated), m_onBackSurfaceCreated);
    m_onBackSurfaceCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DepthSurfaceCreated), m_onDepthSurfaceCreated);
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DepthSurfaceShared), m_onDepthSurfaceCreated);
    m_onDepthSurfaceCreated = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(ChangeTargetViewPort), m_handleChangingViewPort);
    m_handleChangingViewPort = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(ChangeTargetClearingProperty), m_handleChangingClearingProperty);
    m_handleChangingClearingProperty = nullptr;
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

void RenderTarget::OnPrimarySurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (!m_isPrimary) return;
    if (!e) return;
    Graphics::PrimarySurfaceCreated* ev = dynamic_cast<Graphics::PrimarySurfaceCreated*>(e.get());
    if (!ev) return;
    auto back = Graphics::IGraphicAPI::Instance()->TryGetGraphicAsset<Graphics::IBackSurfacePtr>(ev->GetBackSurfaceName());
    if (back) m_backSurface = back.value();
    auto depth = Graphics::IGraphicAPI::Instance()->TryGetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(ev->GetDepthSurfaceName());
    if (depth) m_depthStencilSurface = depth.value();
    if (m_async == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
    {
        AsyncInitialize();
    }
    else
    {
        Initialize();
    }
}

void RenderTarget::OnBackSurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (m_isPrimary) return;
    if (!e) return;
    auto& ev = *e.get();
    if (typeid(ev) == typeid(Graphics::BackSurfaceCreated))
    {
        Graphics::BackSurfaceCreated* evb = dynamic_cast<Graphics::BackSurfaceCreated*>(e.get());
        if (!evb) return;
        if (evb->GetBackSurfaceName() != m_backSurfaceName) return;
    }
    else if (typeid(ev) == typeid(Graphics::MultiBackSurfaceCreated))
    {
        Graphics::MultiBackSurfaceCreated* evb = dynamic_cast<Graphics::MultiBackSurfaceCreated*>(e.get());
        if (!evb) return;
        if (evb->GetBackSurfaceName() != m_backSurfaceName) return;
    }

    auto back = Graphics::IGraphicAPI::Instance()->TryGetGraphicAsset<Graphics::IBackSurfacePtr>(m_backSurfaceName);
    if (back) m_backSurface = back.value();
    if (!m_backSurface) return;
    if (m_async == Graphics::IGraphicAPI::AsyncType::UseAsyncDevice)
    {
        AsyncInitialize();
    }
    else
    {
        Initialize();
    }
}

void RenderTarget::OnDepthSurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto& ev = *e.get();
    if (typeid(ev) == typeid(Graphics::DepthSurfaceCreated))
    {
        Graphics::DepthSurfaceCreated* evd = dynamic_cast<Graphics::DepthSurfaceCreated*>(e.get());
        if (!evd) return;
        if (evd->GetDepthSurfaceName() != m_depthSurfaceName) return;
    }
    else if (typeid(ev) == typeid(Graphics::DepthSurfaceShared))
    {
        Graphics::DepthSurfaceShared* evd = dynamic_cast<Graphics::DepthSurfaceShared*>(e.get());
        if (!evd) return;
        if (evd->GetDepthSurfaceName() != m_depthSurfaceName) return;
    }
    auto depth = Graphics::IGraphicAPI::Instance()->TryGetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(m_depthSurfaceName);
    if (depth) m_depthStencilSurface = depth.value();
    if (!m_depthStencilSurface) return;
    if (m_backSurface)
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
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
