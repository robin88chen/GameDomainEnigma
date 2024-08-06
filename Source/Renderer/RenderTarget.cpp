#include "RenderTarget.h"
#include "RendererErrors.h"
#include "RendererEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDepthStencilSurface.h"
#include "GraphicKernel/IBackSurface.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/Texture.h"
#include "GraphicKernel/ITexture.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/TextureDto.h"
#include "GameEngine/TextureQueries.h"
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

    subscribeHandler();

    if (m_isPrimary)
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreatePrimarySurface>(
            primary_back_surface_name, primary_depth_surface_name));
    }
}

RenderTarget::RenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::BackSurfaceSpecification& back_specification,
    const Graphics::DepthStencilSurfaceSpecification& depth_specification, const std::vector<Graphics::RenderTextureUsage>& usages) : m_dimension{ 1, 1 }
{
    m_isPrimary = false;
    m_name = name;
    m_backSpecification = back_specification;
    m_depthSpecification = depth_specification;
    m_renderTargetTexture = nullptr;
    m_usages = usages;

    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, RenderTargetClear::BothBuffer };

    m_resizingBits.reset();

    subscribeHandler();
    createRenderTargetTexture(render_tex_id);
    initBackSurface(m_backSpecification.value());
    initDepthStencilSurface(m_depthSpecification.value());
}

RenderTarget::RenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::MultiBackSurfaceSpecification& multi_back_specification,
    const std::string& depth_name, const Graphics::IDepthStencilSurfacePtr& depth_surface, const std::vector<Graphics::RenderTextureUsage>& usages) : m_dimension{ 1, 1 }
{
    m_isPrimary = false;
    m_name = name;
    m_multiBackSpecification = multi_back_specification;
    m_renderTargetTexture = nullptr;
    m_usages = usages;

    m_clearingProperty = { MathLib::ColorRGBA(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 0, RenderTargetClear::BothBuffer };

    m_resizingBits.reset();

    subscribeHandler();
    createRenderTargetTexture(render_tex_id);
    initMultiBackSurface(m_multiBackSpecification.value());
    shareDepthStencilSurface(depth_name, depth_surface);
}

RenderTarget::~RenderTarget()
{
    unsubscribeHandler();
    m_renderTargetTexture = nullptr;
    m_backSurface = nullptr;
    m_depthStencilSurface = nullptr;
}

error RenderTarget::initBackSurface(const Graphics::BackSurfaceSpecification& specification)
{
    Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateBacksurface>(specification));

    return ErrorCode::ok;
}

error RenderTarget::initMultiBackSurface(const Graphics::MultiBackSurfaceSpecification& specification)
{
    Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateMultiBacksurface>(specification));

    return ErrorCode::ok;
}

error RenderTarget::initDepthStencilSurface(const Graphics::DepthStencilSurfaceSpecification& specification)
{
    Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateDepthStencilSurface>(specification));

    return ErrorCode::ok;
}

error RenderTarget::shareDepthStencilSurface(const std::string& depth_name,
    const Graphics::IDepthStencilSurfacePtr& surface)
{
    if (surface)
    {
        m_depthSpecification = Graphics::DepthStencilSurfaceSpecification(depth_name, surface->getDimension(), surface->GetFormat());
    }
    else
    {
        m_depthSpecification = std::nullopt;
    }
    Frameworks::CommandBus::enqueue(std::make_shared<Graphics::ShareDepthStencilSurface>(depth_name, surface));

    return ErrorCode::ok;
}

const Enigma::Graphics::TargetViewPort& RenderTarget::getViewPort()
{
    return m_viewPort;
}

error RenderTarget::bind()
{
    if (!m_backSurface)
    {
        Platforms::Debug::Printf("bind render target %s back surface not ready\n", m_name.c_str());
        return ErrorCode::nullBackSurface;
    }
    Graphics::IGraphicAPI::instance()->bind(m_backSurface, m_depthStencilSurface);
    //Frameworks::CommandBus::enqueue(std::make_shared<Graphics::BindBackSurface>(m_backSurface, m_depthStencilSurface));
    return ErrorCode::ok;
}

error RenderTarget::bindViewPort()
{
    Graphics::IGraphicAPI::instance()->bind(m_viewPort);
    //Frameworks::CommandBus::enqueue(std::make_shared<Graphics::BindViewPort>(m_viewPort));
    return ErrorCode::ok;
}

error RenderTarget::clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value, RenderTargetClearingBits flag) const
{
    if (!m_backSurface) return ErrorCode::nullBackSurface;

    Graphics::IGraphicAPI::instance()->clear(
        (flag & (RenderTargetClearingBits)RenderTargetClear::ColorBuffer).any() ? m_backSurface : nullptr,
        (flag & (RenderTargetClearingBits)RenderTargetClear::DepthBuffer).any() ? m_depthStencilSurface : nullptr,
        color, depth_value, stencil_value);
    //Frameworks::CommandBus::enqueue(std::make_shared<Graphics::ClearSurface>(
      //  ((int)flag & (int)BufferClearFlag::ColorBuffer) ? m_backSurface : nullptr,
      //  ((int)flag & (int)BufferClearFlag::DepthBuffer) ? m_depthStencilSurface : nullptr,
      //  color, depth_value, stencil_value));
    return ErrorCode::ok;
}

error RenderTarget::clear()
{
    return clear(m_clearingProperty.m_color, m_clearingProperty.m_depth,
        m_clearingProperty.m_stencil, m_clearingProperty.m_clearingBits);
}

error RenderTarget::flip() const
{
    if (FATAL_LOG_EXPR(!m_isPrimary)) return ErrorCode::flipNotPrimary;
    Graphics::IGraphicAPI::instance()->flip();
    //Frameworks::CommandBus::enqueue(std::make_shared<Graphics::flipBackSurface>());
    return ErrorCode::ok;
}

error RenderTarget::changeClearingProperty(const RenderTargetClearChangingProperty& prop)
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
    Frameworks::EventPublisher::enqueue(std::make_shared<TargetClearingPropertyChanged>(shared_from_this(), m_clearingProperty));

    return ErrorCode::ok;
}

error RenderTarget::resize(const MathLib::Dimension<unsigned>& dimension)
{
    if (FATAL_LOG_EXPR(!m_backSurface)) return ErrorCode::nullBackSurface;
    bool isCurrentBound = false;
    m_resizingBits.reset();
    if (m_depthStencilSurface == nullptr) m_resizingBits |= Resizing::DepthSurfaceBit;  // 沒有 depth 需要 resize, 設定成完成

    if (Graphics::IGraphicAPI::instance()->CurrentBoundBackSurface() == m_backSurface)
    {
        isCurrentBound = true;
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::BindBackSurface>(nullptr, nullptr));
    }
    m_backSurface->ResizeSurface(dimension);
    m_dimension = m_backSurface->getDimension();

    // 好...來...因為back buffer surface實際上已經重新create了,
    // 所以這裡頭的shader resource view要重新綁定
    //todo : render target texture
    if ((m_renderTargetTexture) && (m_renderTargetTexture->getDeviceTexture()))
    {
        m_renderTargetTexture->getDeviceTexture()->asBackSurface(m_backSurface, m_usages);
    }
    if (m_depthStencilSurface)
    {
        m_depthStencilSurface->ResizeSurface(dimension);
    }
    if (isCurrentBound)
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::BindBackSurface>(m_backSurface, m_depthStencilSurface));
    }

    return ErrorCode::ok;
}

void RenderTarget::subscribeHandler()
{
    m_onPrimarySurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->onPrimarySurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::PrimarySurfaceCreated), m_onPrimarySurfaceCreated);
    m_onBackSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->onBackSurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::BackSurfaceCreated), m_onBackSurfaceCreated);
    Frameworks::EventPublisher::subscribe(typeid(Graphics::MultiBackSurfaceCreated), m_onBackSurfaceCreated);
    m_onDepthSurfaceCreated = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->onDepthSurfaceCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceCreated), m_onDepthSurfaceCreated);
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceShared), m_onDepthSurfaceCreated);

    m_onBackSurfaceResized = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->onBackSurfaceResized(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::BackSurfaceResized), m_onBackSurfaceResized);
    m_onDepthSurfaceResized = std::make_shared<Frameworks::EventSubscriber>(
        [=](auto e) { this->onDepthSurfaceResized(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DepthSurfaceResized), m_onDepthSurfaceResized);

    m_onTextureHydrated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onTextureHydrated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Engine::TextureHydrated), m_onTextureHydrated);
    m_onHydrateTextureFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onHydrateTextureFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Engine::HydrateTextureFailed), m_onHydrateTextureFailed);
}

void RenderTarget::unsubscribeHandler()
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

    Frameworks::EventPublisher::unsubscribe(typeid(Engine::TextureHydrated), m_onTextureHydrated);
    m_onTextureHydrated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Engine::HydrateTextureFailed), m_onHydrateTextureFailed);
    m_onHydrateTextureFailed = nullptr;
}

void RenderTarget::createRenderTargetTexture(const Engine::TextureId& texture_id)
{
    if (m_isPrimary) return;
    if ((!m_backSpecification) && (!m_multiBackSpecification)) return;
    Engine::TextureDto dto;
    if (m_backSpecification.has_value())
    {
        dto.id(texture_id);
        dto.dimension(m_backSpecification->dimension());
        dto.surfaceCount(1);
        dto.format(m_backSpecification->format());
    }
    else if (m_multiBackSpecification.has_value())
    {
        dto.id(texture_id);
        dto.dimension(m_multiBackSpecification->dimension());
        dto.surfaceCount(m_multiBackSpecification->surfaceCount());
        dto.format(m_multiBackSpecification->formats()[0]);
    }
    m_renderTargetTexture = std::make_shared<Engine::RequestTextureConstitution>(dto.id(), dto.toGenericDto())->dispatch();
}

void RenderTarget::initViewPortSize()
{
    m_viewPort.Width() = m_dimension.m_width;
    m_viewPort.Height() = m_dimension.m_height;

    Frameworks::EventPublisher::enqueue(std::make_shared<TargetViewPortInitialized>(shared_from_this(), m_viewPort));
}

void RenderTarget::setViewPort(const Graphics::TargetViewPort& vp)
{
    m_viewPort = vp;
    Frameworks::EventPublisher::enqueue(std::make_shared<TargetViewPortChanged>(shared_from_this(), m_viewPort));
}

void RenderTarget::onPrimarySurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (!m_isPrimary) return;
    if (!e) return;
    Graphics::PrimarySurfaceCreated* ev = dynamic_cast<Graphics::PrimarySurfaceCreated*>(e.get());
    if (!ev) return;
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(ev->GetBackSurfaceName()))
    {
        m_backSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IBackSurfacePtr>(ev->GetBackSurfaceName());
        m_backSpecification = Graphics::BackSurfaceSpecification(ev->GetBackSurfaceName(), m_backSurface->getDimension(), m_backSurface->GetFormat());
    }
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(ev->GetDepthSurfaceName()))
    {
        m_depthStencilSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(ev->GetDepthSurfaceName());
        m_depthSpecification = Graphics::DepthStencilSurfaceSpecification(ev->GetDepthSurfaceName(), m_depthStencilSurface->getDimension(), m_depthStencilSurface->GetFormat());
    }
    completeSurfaceCreation();
}

void RenderTarget::onBackSurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (m_isPrimary) return;
    if ((!m_backSpecification) && (!m_multiBackSpecification)) return;
    if (!e) return;
    auto& ev = *e.get();
    if (typeid(ev) == typeid(Graphics::BackSurfaceCreated))
    {
        Graphics::BackSurfaceCreated* evb = dynamic_cast<Graphics::BackSurfaceCreated*>(e.get());
        if (!evb) return;
        if ((!m_backSpecification.has_value()) || (evb->GetBackSurfaceName() != m_backSpecification.value().name())) return;
    }
    else if (typeid(ev) == typeid(Graphics::MultiBackSurfaceCreated))
    {
        Graphics::MultiBackSurfaceCreated* evb = dynamic_cast<Graphics::MultiBackSurfaceCreated*>(e.get());
        if (!evb) return;
        if ((!m_multiBackSpecification.has_value()) || (evb->GetBackSurfaceName() != m_multiBackSpecification.value().name())) return;
    }

    if (backSurfaceName().empty()) return;

    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(backSurfaceName()))
    {
        m_backSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IBackSurfacePtr>(backSurfaceName());
    }
    if (!m_backSurface) return;
    completeSurfaceCreation();
}

void RenderTarget::onDepthSurfaceCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    if (!m_depthSpecification) return;
    auto& ev = *e.get();
    if (typeid(ev) == typeid(Graphics::DepthSurfaceCreated))
    {
        Graphics::DepthSurfaceCreated* evd = dynamic_cast<Graphics::DepthSurfaceCreated*>(e.get());
        if (!evd) return;
        if (evd->GetDepthSurfaceName() != m_depthSpecification.value().name()) return;
    }
    else if (typeid(ev) == typeid(Graphics::DepthSurfaceShared))
    {
        Graphics::DepthSurfaceShared* evd = dynamic_cast<Graphics::DepthSurfaceShared*>(e.get());
        if (!evd) return;
        if (evd->GetDepthSurfaceName() != m_depthSpecification.value().name()) return;
    }
    if (Graphics::IGraphicAPI::instance()->HasGraphicAsset(m_depthSpecification.value().name()))
    {
        m_depthStencilSurface = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IDepthStencilSurfacePtr>(m_depthSpecification.value().name());
    }
    if (!m_depthStencilSurface) return;
    completeSurfaceCreation();
}

void RenderTarget::onBackSurfaceResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::BackSurfaceResized, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetSurfaceName() != backSurfaceName()) return;
    m_dimension = m_backSurface->getDimension();
    m_resizingBits |= Resizing::BackSurfaceBit;
    if (m_resizingBits.all())
    {
        initViewPortSize();
        Frameworks::EventPublisher::enqueue(std::make_shared<RenderTargetResized>(shared_from_this(), m_dimension));
    }
}

void RenderTarget::onDepthSurfaceResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Graphics::DepthSurfaceResized, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetSurfaceName() != depthSurfaceName()) return;
    m_resizingBits |= Resizing::DepthSurfaceBit;
    if (m_resizingBits.all())
    {
        initViewPortSize();
        Frameworks::EventPublisher::enqueue(std::make_shared<RenderTargetResized>(shared_from_this(), m_dimension));
    }
}

void RenderTarget::onTextureHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Engine::TextureHydrated>(e);
    if (!ev) return;
    if (!m_backSurface) return;
    if (!m_renderTargetTexture) return;
    if (ev->id() != m_renderTargetTexture->id()) return;
    completeRenderTextureHydration();
}

void RenderTarget::onHydrateTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Engine::HydrateTextureFailed>(e);
    if (!ev) return;
    if (!m_renderTargetTexture) return;
    if (ev->id() != m_renderTargetTexture->id()) return;
    Frameworks::EventPublisher::enqueue(std::make_shared<CreateRenderTargetTextureFailed>(ev->id().name(), ev->error()));
}

void RenderTarget::completeSurfaceCreation()
{
    if (!m_backSurface) return;
    if (m_backSurface && m_depthStencilSurface)
    {
        m_depthStencilSurface->MakeBackSurfaceRelated(m_backSurface);
    }
    m_dimension = m_backSurface->getDimension();
    initViewPortSize();
    if (m_isPrimary)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<PrimaryRenderTargetCreated>(m_name, shared_from_this()));
    }
}

void RenderTarget::completeRenderTextureHydration()
{
    if (!m_renderTargetTexture) return;
    if (!m_renderTargetTexture->getDeviceTexture()) return;
    if (!m_backSurface) return;

    m_renderTargetTexture->getDeviceTexture()->asBackSurface(m_backSurface, m_usages);
    Frameworks::EventPublisher::enqueue(std::make_shared<RenderTargetTextureCreated>(shared_from_this(), m_renderTargetTexture->id().name()));
}

std::string RenderTarget::backSurfaceName() const
{
    if (m_backSpecification.has_value())
    {
        return m_backSpecification.value().name();
    }
    else if (m_multiBackSpecification.has_value())
    {
        return m_multiBackSpecification.value().name();
    }
    return "";
}

std::string RenderTarget::depthSurfaceName() const
{
    if (m_depthSpecification.has_value())
    {
        return m_depthSpecification.value().name();
    }
    return "";
}
