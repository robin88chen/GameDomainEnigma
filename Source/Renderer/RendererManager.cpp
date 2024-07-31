#include "RendererManager.h"
#include "RendererErrors.h"
#include "RendererCommands.h"
#include "RendererEvents.h"
#include "Renderer.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryMacro.h"
#include <cassert>

#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RendererManager, ISystemService);

RendererManager::CustomRendererFactoryTable RendererManager::m_customRendererFactoryTable;

RendererManager::RendererManager(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_accumulateRendererStamp = 0;
}

RendererManager::~RendererManager()
{
    assert(m_renderers.empty());
}

Enigma::Frameworks::ServiceResult RendererManager::onInit()
{
    m_accumulateRendererStamp = 0;

    m_createRenderer =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->createRenderer(c); });
    Frameworks::CommandBus::subscribe(typeid(CreateRenderer), m_createRenderer);
    m_destroyRenderer =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->destroyRenderer(c); });
    Frameworks::CommandBus::subscribe(typeid(DestroyRenderer), m_destroyRenderer);

    m_createRenderTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->createRenderTarget(c); });
    Frameworks::CommandBus::subscribe(typeid(CreateRenderTarget), m_createRenderTarget);
    m_destroyRenderTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->destroyRenderTarget(c); });
    Frameworks::CommandBus::subscribe(typeid(DestroyRenderTarget), m_destroyRenderTarget);

    m_resizePrimaryTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->resizePrimaryTarget(c); });
    Frameworks::CommandBus::subscribe(typeid(ResizePrimaryRenderTarget), m_resizePrimaryTarget);

    m_changeViewPort =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->changeViewPort(c); });
    Frameworks::CommandBus::subscribe(typeid(ChangeTargetViewPort), m_changeViewPort);
    m_changeClearingProperty =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->changeClearingProperty(c); });
    Frameworks::CommandBus::subscribe(typeid(ChangeTargetClearingProperty), m_changeClearingProperty);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RendererManager::onTerm()
{
    Frameworks::CommandBus::unsubscribe(typeid(CreateRenderer), m_createRenderer);
    m_createRenderer = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(DestroyRenderer), m_destroyRenderer);
    m_destroyRenderer = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(CreateRenderTarget), m_createRenderTarget);
    m_createRenderTarget = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(DestroyRenderTarget), m_destroyRenderTarget);
    m_destroyRenderTarget = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(ResizePrimaryRenderTarget), m_resizePrimaryTarget);
    m_resizePrimaryTarget = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(ChangeTargetViewPort), m_changeViewPort);
    m_changeViewPort = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(ChangeTargetClearingProperty), m_changeClearingProperty);
    m_changeClearingProperty = nullptr;

    removeAllRenderer();
    removeAllRenderTarget();
    m_accumulateRendererStamp = 0;
    return Frameworks::ServiceResult::Complete;
}
void RendererManager::registerCustomRendererFactory(const std::string& type_name, const CustomRendererFactoryFunc& fn)
{
    m_customRendererFactoryTable.emplace(type_name, fn);
}

error RendererManager::createRenderer(const std::string& name)
{
    IRendererPtr render = getRenderer(name);
    if (render)
    {
        // render already exist
        return ErrorCode::rendererAlreadyExist;
    }
    render = std::make_shared<Renderer>(name);
    m_renderers.emplace(name, render);
    assert(m_renderers.size() <= 32);

    unsigned int stamp = 1;
    while (m_accumulateRendererStamp & stamp)
    {
        stamp = stamp << 1;
    }
    m_accumulateRendererStamp |= stamp;
    render->stampBitMask(stamp);

    Frameworks::EventPublisher::enqueue(std::make_shared<RendererCreated>(render->name(), render));

    return ErrorCode::ok;
}

error RendererManager::createCustomRenderer(const std::string& type_name, const std::string& name)
{
    IRendererPtr render = getRenderer(name);
    if (render)
    {
        // render already exist
        return ErrorCode::rendererAlreadyExist;
    }
    // new renderer by factory
    CustomRendererFactoryTable::iterator iter = m_customRendererFactoryTable.find(type_name);
    if (iter == m_customRendererFactoryTable.end()) return ErrorCode::rendererFactoryFail;
    render = (iter->second)(name);
    m_renderers.emplace(name, render);
    assert(m_renderers.size() <= 32);

    unsigned int stamp = 1;
    while (m_accumulateRendererStamp & stamp)
    {
        stamp = stamp << 1;
    }
    m_accumulateRendererStamp |= stamp;
    render->stampBitMask(stamp);

    Frameworks::EventPublisher::enqueue(std::make_shared<RendererCreated>(render->name(), render));

    return ErrorCode::ok;
}

error RendererManager::insertRenderer(const std::string& name, const IRendererPtr& renderer)
{
    IRendererPtr render = getRenderer(name);
    if (render)
    {
        // render already exist
        return ErrorCode::rendererAlreadyExist;
    }
    m_renderers.emplace(name, renderer);
    assert(m_renderers.size() <= 32);
    unsigned int stamp = 1;
    while (m_accumulateRendererStamp & stamp)
    {
        stamp = stamp << 1;
    }
    m_accumulateRendererStamp |= stamp;
    renderer->stampBitMask(stamp);
    Frameworks::EventPublisher::enqueue(std::make_shared<RendererCreated>(renderer->name(), renderer));
    return ErrorCode::ok;
}

error RendererManager::destroyRenderer(const std::string& name)
{
    const IRendererPtr render = getRenderer(name);
    if (!render) return ErrorCode::rendererNotExist;

    const unsigned int stamp = render->stampBitMask();
    m_accumulateRendererStamp &= (~stamp);
    m_renderers.erase(name);

    Frameworks::EventPublisher::enqueue(std::make_shared<RendererDestroyed>(name));

    return ErrorCode::ok;
}

IRendererPtr RendererManager::getRenderer(const std::string& name) const
{
    const auto it = m_renderers.find(name);
    if (it == m_renderers.end()) return nullptr;
    return it->second;
}

error RendererManager::createRenderTarget(const std::string& name, RenderTarget::PrimaryType primary, const std::vector<Graphics::RenderTextureUsage>& usages)
{
    if (auto target_check = getRenderTarget(name))
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, primary, usages) };
    m_renderTargets.emplace(name, target);

    if (primary == RenderTarget::PrimaryType::IsPrimary)
    {
        m_primaryRenderTargetName = name;
    }

    return ErrorCode::ok;
}

error RendererManager::createRenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::BackSurfaceSpecification& back_specification, const Graphics::DepthStencilSurfaceSpecification& depth_specification, const std::vector<Graphics::RenderTextureUsage>& usages)
{
    if (auto target_check = getRenderTarget(name))
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, render_tex_id, back_specification, depth_specification, usages) };
    m_renderTargets.emplace(name, target);

    return ErrorCode::ok;
}

error RendererManager::createRenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::MultiBackSurfaceSpecification& back_specification, const std::string& depth_name, const Graphics::IDepthStencilSurfacePtr& depth_surface, const std::vector<Graphics::RenderTextureUsage>& usages)
{
    if (auto target_check = getRenderTarget(name))
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, render_tex_id, back_specification, depth_name, depth_surface, usages) };
    m_renderTargets.emplace(name, target);

    return ErrorCode::ok;
}

error RendererManager::destroyRenderTarget(const std::string& name)
{
    const auto target = getRenderTarget(name);
    if (!target) return ErrorCode::renderTargetNotExist;
    m_renderTargets.erase(name);

    Frameworks::EventPublisher::enqueue(std::make_shared<RenderTargetDestroyed>(name));

    return ErrorCode::ok;
}

RenderTargetPtr RendererManager::getRenderTarget(const std::string& name) const
{
    RenderTargetMap::const_iterator iter = m_renderTargets.find(name);
    if (iter == m_renderTargets.end()) return nullptr;
    return iter->second;
}

RenderTargetPtr RendererManager::getPrimaryRenderTarget() const
{
    return getRenderTarget(m_primaryRenderTargetName);
}

void RendererManager::resizePrimaryTarget(const Frameworks::ICommandPtr& c) const
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ResizePrimaryRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    const auto target = getPrimaryRenderTarget();
    if (!target) return;
    target->resize(cmd->dimension());
}

void RendererManager::removeAllRenderer()
{
    if (m_renderers.size() == 0) return;
    m_renderers.clear();

    m_accumulateRendererStamp = 0;
}

void RendererManager::removeAllRenderTarget()
{
    if (m_renderTargets.size() == 0) return;
    m_renderTargets.clear();
}

void RendererManager::createRenderer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateRenderer, Frameworks::ICommand>(c);
    if (!cmd) return;
    createRenderer(cmd->rendererName());
}

void RendererManager::destroyRenderer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DestroyRenderer, Frameworks::ICommand>(c);
    if (!cmd) return;
    destroyRenderer(cmd->rendererName());
}

void RendererManager::createRenderTarget(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    createRenderTarget(cmd->renderTargetName(), cmd->primaryType(), cmd->usages());
}

void RendererManager::destroyRenderTarget(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DestroyRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    destroyRenderTarget(cmd->renderTargetName());
}

void RendererManager::changeViewPort(const Frameworks::ICommandPtr& c) const
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeTargetViewPort, Frameworks::ICommand>(c);
    if (!cmd) return;
    const auto target = getRenderTarget(cmd->renderTargetName());
    if (!target) return;
    target->setViewPort(cmd->viewPort());
}

void RendererManager::changeClearingProperty(const Frameworks::ICommandPtr& c) const
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeTargetClearingProperty, Frameworks::ICommand>(c);
    if (!cmd) return;
    const auto target = getRenderTarget(cmd->renderTargetName());
    if (!target) return;
    target->changeClearingProperty(cmd->property());
}
