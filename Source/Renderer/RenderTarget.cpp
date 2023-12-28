#include "RenderTarget.h"
#include "RendererErrors.h"
#include "RendererEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/IBackSurface.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/Texture.h"
#include "GraphicKernel/ITexture.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/TextureDto.h"
#include <cassert>

using namespace Enigma::Renderer;

const std::string primary_back_surface_name = "primary_back_surface";
const std::string primary_depth_surface_name = "primary_depth_surface";

RenderTarget::RenderTarget(const std::string& name, PrimaryType primary,
    const std::vector<Graphics::RenderTextureUsage>& usages) : m_dimension{ 1, 1 }
{
    m_isPrimary = primary == PrimaryType::IsPrimary;
    m_name = name;
    m_renderTargetTexture = nullptr;
    m_usages = usages;

    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, RenderTargetClear::BothBuffer };

    m_resizingBits.reset();

    SubscribeHandler();

    if (m_isPrimary)
    {
        Frameworks::CommandBus::post(std::make_shared<Graphics::CreatePrimarySurface>(
            primary_back_surface_name, primary_depth_surface_name));
    }
}

RenderTarget::RenderTarget(const std::string& name, const std::vector<Graphics::RenderTextureUsage>& usages)
    : m_dimension{ 1, 1 }
{
    m_isPrimary = false;
    m_name = name;
    m_renderTargetTexture = nullptr;
    m_usages = usages;

    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, RenderTargetClear::BothBuffer };

    SubscribeHandler();
}

RenderTarget::~RenderTarget()
{
    UnsubscribeHandler();
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
    if (!m_isPrimary)
    {
        CreateRenderTargetTexture();
    }
    else
    {
        Frameworks::EventPublisher::post(std::make_shared<PrimaryRenderTargetCreated>(m_name, shared_from_this()));
    }
    InitViewPortSize();
    return ErrorCode::ok;
}

future_error RenderTarget::AsyncInitialize()
{
    return Graphics::IGraphicAPI::instance()->GetGraphicThread()
        ->PushTask([lifetime = shared_from_this(), this]() -> error { return Initialize(); });
}

error RenderTarget::InitBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    const Graphics::GraphicFormat& fmt)
{
    m_backSurfaceName = back_name;
    Frameworks::CommandBus::post(std::make_shared<Graphics::CreateBacksurface>(back_name, dimension, fmt));

    return ErrorCode::ok;
}

error RenderTarget::InitMultiBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    m_backSurfaceName = back_name;
    Frameworks::CommandBus::post(std::make_shared<Graphics::CreateMultiBacksurface>(back_name, dimension, surface_count, fmts));

    return ErrorCode::ok;
}

error RenderTarget::InitDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension,
    const Graphics::GraphicFormat& fmt)
{
    m_depthSurfaceName = depth_name;
    Frameworks::CommandBus::post(std::make_shared<Graphics::CreateDepthStencilSurface>(depth_name, dimension, fmt));

    return ErrorCode::ok;
}

error RenderTarget::ShareDepthStencilSurface(const std::string& depth_name,
    const Graphics::IDepthStencilSurfacePtr& surface)
{
    m_depthSurfaceName = depth_name;
    Frameworks::CommandBus::post(std::make_shared<Graphics::ShareDepthStencilSurface>(depth_name, surface));

    return ErrorCode::ok;
}

const Enigma::Graphics::TargetViewPort& RenderTarget::GetViewPort()
{
    return m_viewPort;
}

error RenderTarget::Bind()
{
    if (!m_backSurface)
    {
        Platforms::Debug::Printf("bind render target %s back surface not ready\n", m_name.c_str());
        return ErrorCode::nullBackSurface;
    }
    Graphics::IGraphicAPI::instance()->Bind(m_backSurface, m_depthStencilSurface);
    //Frameworks::CommandBus::post(std::make_shared<Graphics::BindBackSurface>(m_backSurface, m_depthStencilSurface));
    return ErrorCode::ok;
}

error RenderTarget::BindViewPort()
{
    Graphics::IGraphicAPI::instance()->Bind(m_viewPort);
    //Frameworks::CommandBus::post(std::make_shared<Graphics::BindViewPort>(m_viewPort));
    return ErrorCode::ok;
}

error RenderTarget::Clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value, RenderTargetClearingBits flag) const
{
    if (!m_backSurface) return ErrorCode::nullBackSurface;

    Graphics::IGraphicAPI::instance()->Clear(
        (flag & (RenderTargetClearingBits)RenderTargetClear::ColorBuffer).any() ? m_backSurface : nullptr,
        (flag & (RenderTargetClearingBits)RenderTargetClear::DepthBuffer).any() ? m_depthStencilSurface : nullptr,
        color, depth_value, stencil_value);
    //Frameworks::CommandBus::post(std::make_shared<Graphics::ClearSurface>(
      //  ((int)flag & (int)BufferClearFlag::ColorBuffer) ? m_backSurface : nullptr,
      //  ((int)flag & (int)BufferClearFlag::DepthBuffer) ? m_depthStencilSurface : nullptr,
      //  color, depth_value, stencil_value));
    return ErrorCode::ok;
}

error RenderTarget::Clear()
{
    return Clear(m_clearingProperty.m_color, m_clearingProperty.m_depth,
        m_clearingProperty.m_stencil, m_clearingProperty.m_clearingBits);
}

error RenderTarget::Flip() const
{
    if (FATAL_LOG_EXPR(!m_isPrimary)) return ErrorCode::flipNotPrimary;
    Graphics::IGraphicAPI::instance()->Flip();
    //Frameworks::CommandBus::post(std::make_shared<Graphics::FlipBackSurface>());
    return ErrorCode::ok;
}

error RenderTarget::ChangeClearingProperty(const RenderTargetClearChangingProperty& prop)
{
    if (prop.m_color)
    {
        m_clearingProperty.m_color = prop.m_color.value();
    }
    if (prop.m_depth)
    {
        m_clearingProperty.m_depth = prop.m_depth.value();
    }
    if (prop.m_stencil)
    {
        m_clearingProperty.m_stencil = prop.m_stencil.value();
    }
    if (prop.m_clearingBits)
    {
        m_clearingProperty.m_clearingBits = prop.m_clearingBits.value();
    }
    Frameworks::EventPublisher::post(std::make_shared<TargetClearingPropertyChanged>(shared_from_this(), m_clearingProperty));

    return ErrorCode::ok;
}

error RenderTarget::Resize(const MathLib::Dimension<unsigned>& dimension)
{
    if (FATAL_LOG_EXPR(!m_backSurface)) return ErrorCode::nullBackSurface;
    bool isCurrentBound = false;
    m_resizingBits.reset();
    if (m_depthStencilSurface == nullptr) m_resizingBits |= Resizing::DepthSurfaceBit;  // 沒有 depth 需要 resize, 設定成完成

    if (Graphics::IGraphicAPI::instance()->CurrentBoundBackSurface() == m_backSurface)
    {
        isCurrentBound = true;
        Frameworks::CommandBus::post(std::make_shared<Graphics::BindBackSurface>(nullptr, nullptr));
    }
    m_backSurface->ResizeSurface(dimension);
    m_dimension = m_backSurface->GetDimension();

    // 好...來...因為back buffer surface實際上已經重新create了,
    // 所以這裡頭的shader resource view要重新綁定
    //todo : render target texture
    if (m_renderTargetTexture)
    {
        m_renderTargetTexture->getDeviceTexture()->AsBackSurface(m_backSurface, m_usages);
    }
    if (m_depthStencilSurface)
    {
        m_depthStencilSurface->ResizeSurface(dimension);
    }
    if (isCurrentBound)
    {
        Frameworks::CommandBus::post(std::make_shared<Graphics::BindBackSurface>(m_backSurface, m_depthStencilSurface));
    }

    return ErrorCode::ok;
}

void RenderTarget::SubscribeHandler()
{
    m_onPrimarySurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnPrimarySurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::PrimarySurfaceCreated), m_onPrimarySurfaceCreated);
    m_onBackSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnBackSurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::BackSurfaceCreated), m_onBackSurfaceCreated);
    Frameworks::EventPublisher::subscribe(typeid(Graphics::MultiBackSurfaceCreated), m_onBackSurfaceCreated);
    m_onDepthSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnDepthSurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceCreated), m_onDepthSurfaceCreated);
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceShared), m_onDepthSurfaceCreated);

    m_onBackSurfaceResized = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnBackSurfaceResized(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::BackSurfaceResized), m_onBackSurfaceResized);
    m_onDepthSurfaceResized = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->OnDepthSurfaceResized(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceResized), m_onDepthSurfaceResized);

    m_onTextureContented = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureContented(e); });
    Frameworks::EventPublisher::subscribe(typeid(Engine::TextureContented), m_onTextureContented);
    m_onContentTextureFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onContentTextureFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Engine::ContentTextureFailed), m_onContentTextureFailed);
}

void RenderTarget::UnsubscribeHandler()
{
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::PrimarySurfaceCreated), m_onPrimarySurfaceCreated);
    m_onPrimarySurfaceCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::BackSurfaceCreated), m_onBackSurfaceCreated);
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::MultiBackSurfaceCreated), m_onBackSurfaceCreated);
    m_onBackSurfaceCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DepthSurfaceCreated), m_onDepthSurfaceCreated);
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DepthSurfaceShared), m_onDepthSurfaceCreated);
    m_onDepthSurfaceCreated = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::BackSurfaceResized), m_onBackSurfaceResized);
    m_onBackSurfaceResized = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DepthSurfaceResized), m_onDepthSurfaceResized);
    m_onDepthSurfaceResized = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Engine::TextureContented), m_onTextureContented);
    m_onTextureContented = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Engine::ContentTextureFailed), m_onContentTextureFailed);
    m_onContentTextureFailed = nullptr;
}

void RenderTarget::CreateRenderTargetTexture()
{
    if (m_isPrimary) return;
    if (!m_backSurface) return;
    Engine::TextureDto dto;
    dto.id() = m_backSurface->getName();
    dto.dimension() = m_backSurface->GetDimension();
    dto.surfaceCount() = m_backSurface->GetSurfaceCount();
    dto.format() = m_backSurface->GetFormat();
    Frameworks::CommandBus::post(std::make_shared<Engine::ConstituteTexture>(dto.id(), dto.toGenericDto()));
}

void RenderTarget::InitViewPortSize()
{
    m_viewPort.Width() = m_dimension.m_width;
    m_viewPort.Height() = m_dimension.m_height;

    Frameworks::EventPublisher::post(std::make_shared<TargetViewPortInitialized>(shared_from_this(), m_viewPort));
}

void RenderTarget::SetViewPort(const Graphics::TargetViewPort& vp)
{
    m_viewPort = vp;
    Frameworks::EventPublisher::post(std::make_shared<TargetViewPortChanged>(shared_from_this(), m_viewPort));
}

std::optional<unsigned> RenderTarget::FindRenderTextureUsageIndex(Graphics::RenderTextureUsage usage) const
{
    for (unsigned i = 0; i < m_usages.size(); ++i)
    {
        if (m_usages[i] == usage) return i;
    }
    return std::nullopt;
}

void RenderTarget::OnPrimarySurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (!m_isPrimary) return;
    if (!e) return;
    Graphics::PrimarySurfaceCreated* ev = dynamic_cast<Graphics::PrimarySurfaceCreated*>(e.get());
    if (!ev) return;
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(ev->GetBackSurfaceName()))
    {
        m_backSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IBackSurfacePtr>(ev->GetBackSurfaceName());
        m_backSurfaceName = ev->GetBackSurfaceName();
    }
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(ev->GetDepthSurfaceName()))
    {
        m_depthStencilSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(ev->GetDepthSurfaceName());
        m_depthSurfaceName = ev->GetDepthSurfaceName();
    }
    if (Graphics::IGraphicAPI::instance()->UseAsync())
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

    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(m_backSurfaceName))
    {
        m_backSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IBackSurfacePtr>(m_backSurfaceName);
    }
    if (!m_backSurface) return;
    if (Graphics::IGraphicAPI::instance()->UseAsync())
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
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(m_depthSurfaceName))
    {
        m_depthStencilSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(m_depthSurfaceName);
    }
    if (!m_depthStencilSurface) return;
    if (m_backSurface)
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
}

void RenderTarget::OnBackSurfaceResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::BackSurfaceResized, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetSurfaceName() != m_backSurfaceName) return;
    m_dimension = m_backSurface->GetDimension();
    m_resizingBits |= Resizing::BackSurfaceBit;
    if (m_resizingBits.all())
    {
        InitViewPortSize();
        Frameworks::EventPublisher::post(std::make_shared<RenderTargetResized>(shared_from_this(), m_dimension));
    }
}

void RenderTarget::OnDepthSurfaceResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::DepthSurfaceResized, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetSurfaceName() != m_depthSurfaceName) return;
    m_resizingBits |= Resizing::DepthSurfaceBit;
    if (m_resizingBits.all())
    {
        InitViewPortSize();
        Frameworks::EventPublisher::post(std::make_shared<RenderTargetResized>(shared_from_this(), m_dimension));
    }
}

void RenderTarget::onTextureContented(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Engine::TextureContented>(e);
    if (!ev) return;
    if (!m_backSurface) return;
    if (ev->id().name() != m_backSurface->getName()) return;
    m_renderTargetTexture = ev->texture();
    m_renderTargetTexture->getDeviceTexture()->AsBackSurface(m_backSurface, m_usages);
    Frameworks::EventPublisher::post(std::make_shared<RenderTargetTextureCreated>(shared_from_this(), ev->id().name()));
}

void RenderTarget::onContentTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Engine::ContentTextureFailed>(e);
    if (!ev) return;
    if (!m_backSurface) return;
    if (ev->id().name() != m_backSurface->getName()) return;
    Frameworks::EventPublisher::post(std::make_shared<CreateRenderTargetTextureFailed>(ev->id().name(), ev->error()));
}
