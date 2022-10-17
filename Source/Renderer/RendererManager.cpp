#include "RendererManager.h"
#include "RendererErrors.h"
#include "RendererCommands.h"
#include "RendererEvents.h"
#include "Renderer.h"
#include "RenderElementBuilder.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

#include "Platforms/PlatformLayer.h"


using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RendererManager);

RendererManager::CustomRendererFactoryTable RendererManager::m_customRendererFactoryTable;

RendererManager::RendererManager(Frameworks::ServiceManager* srv_mngr, Engine::RenderBufferRepository* buffer_repository,
    Engine::EffectMaterialManager* material_manager) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, Renderer, RendererManager, ISystemService);
    m_needTick = false;
    m_accumulateRendererStamp = 0;
    m_elementBuilder = menew RenderElementBuilder(this, buffer_repository, material_manager);
}

RendererManager::~RendererManager()
{
    assert(m_renderers.empty());
    delete m_elementBuilder;
}

Enigma::Frameworks::ServiceResult RendererManager::OnInit()
{
    m_accumulateRendererStamp = 0;

    m_doCreatingRenderer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingRenderer(c); });
    Frameworks::CommandBus::Subscribe(typeid(Enigma::Renderer::CreateRenderer), m_doCreatingRenderer);
    m_doDestroyingRenderer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoDestroyingRenderer(c); });
    Frameworks::CommandBus::Subscribe(typeid(Enigma::Renderer::DestroyRenderer), m_doDestroyingRenderer);

    m_doCreatingRenderTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingRenderTarget(c); });
    Frameworks::CommandBus::Subscribe(typeid(Enigma::Renderer::CreateRenderTarget), m_doCreatingRenderTarget);
    m_doDestroyingRenderTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoDestroyingRenderTarget(c); });
    Frameworks::CommandBus::Subscribe(typeid(Enigma::Renderer::DestroyRenderTarget), m_doDestroyingRenderTarget);

    m_doResizingPrimaryTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoResizingPrimaryTarget(c); });
    Frameworks::CommandBus::Subscribe(typeid(ResizePrimaryRenderTarget), m_doResizingPrimaryTarget);
    m_doBuildingRenderElement =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBuildingRenderElement(c); });
    Frameworks::CommandBus::Subscribe(typeid(Enigma::Renderer::BuildRenderElement), m_doBuildingRenderElement);

    m_onBuilderRenderElementBuilt =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnBuilderRenderElementBuilt(c); });
    Frameworks::EventPublisher::Subscribe(typeid(RenderElementBuilder::RenderElementBuilt), m_onBuilderRenderElementBuilt);
    m_onBuildRenderElementFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnBuildRenderElementFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(BuildRenderElementFailed), m_onBuildRenderElementFailed);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RendererManager::OnTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_elementBuilder);
    m_elementBuilder->BuildRenderElement(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult RendererManager::OnTerm()
{
    Frameworks::CommandBus::Unsubscribe(typeid(Enigma::Renderer::CreateRenderer), m_doCreatingRenderer);
    m_doCreatingRenderer = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Enigma::Renderer::DestroyRenderer), m_doDestroyingRenderer);
    m_doDestroyingRenderer = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Enigma::Renderer::CreateRenderTarget), m_doCreatingRenderTarget);
    m_doCreatingRenderTarget = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Enigma::Renderer::DestroyRenderTarget), m_doDestroyingRenderTarget);
    m_doDestroyingRenderTarget = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(ResizePrimaryRenderTarget), m_doResizingPrimaryTarget);
    m_doResizingPrimaryTarget = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Enigma::Renderer::BuildRenderElement), m_doBuildingRenderElement);
    m_doBuildingRenderElement = nullptr;

    Frameworks::EventPublisher::Unsubscribe(typeid(RenderElementBuilder::RenderElementBuilt), m_onBuilderRenderElementBuilt);
    m_onBuilderRenderElementBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(BuildRenderElementFailed), m_onBuildRenderElementFailed);
    m_onBuildRenderElementFailed = nullptr;

    ClearAllRenderer();
    ClearAllRenderTarget();
    m_accumulateRendererStamp = 0;
    return Frameworks::ServiceResult::Complete;
}
void RendererManager::RegisterCustomRendererFactory(const std::string& type_name, CustomRendererFactoryFunc fn)
{
    m_customRendererFactoryTable.emplace(type_name, fn);
}

error RendererManager::CreateRenderer(const std::string& name)
{
    IRendererPtr render = GetRenderer(name);
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
    render->SetStampBitMask(stamp);

    Frameworks::EventPublisher::Post(std::make_shared<RendererCreated>(render->GetName(), render));

    return ErrorCode::ok;
}

error RendererManager::CreateCustomRenderer(const std::string& type_name, const std::string& name)
{
    IRendererPtr render = GetRenderer(name);
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
    render->SetStampBitMask(stamp);

    Frameworks::EventPublisher::Post(std::make_shared<RendererCreated>(render->GetName(), render));

    return ErrorCode::ok;
}

error RendererManager::DestroyRenderer(const std::string& name)
{
    IRendererPtr render = GetRenderer(name);
    if (!render) return ErrorCode::rendererNotExist;

    unsigned int stamp = render->GetStampBitMask();
    m_accumulateRendererStamp &= (~stamp);
    m_renderers.erase(name);

    Frameworks::EventPublisher::Post(std::make_shared<RendererDestroyed>(name));

    return ErrorCode::ok;
}

IRendererPtr RendererManager::GetRenderer(const std::string& name) const
{
    auto it = m_renderers.find(name);
    if (it == m_renderers.end()) return nullptr;
    return it->second;
}

error RendererManager::CreateRenderTarget(const std::string& name, RenderTarget::PrimaryType primary)
{
    if (auto target_check = GetRenderTarget(name))
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, primary) };
    m_renderTargets.emplace(name, target);

    if (primary == RenderTarget::PrimaryType::IsPrimary)
    {
        m_primaryRenderTargetName = name;
    }

    return ErrorCode::ok;
}

error RendererManager::DestroyRenderTarget(const std::string& name)
{
    auto target = GetRenderTarget(name);
    if (!target) return ErrorCode::renderTargetNotExist;
    m_renderTargets.erase(name);

    Frameworks::EventPublisher::Post(std::make_shared<RenderTargetDestroyed>(name));

    return ErrorCode::ok;
}

RenderTargetPtr RendererManager::GetRenderTarget(const std::string& name) const
{
    RenderTargetMap::const_iterator iter = m_renderTargets.find(name);
    if (iter == m_renderTargets.end()) return nullptr;
    return iter->second;
}

RenderTargetPtr RendererManager::GetPrimaryRenderTarget() const
{
    return GetRenderTarget(m_primaryRenderTargetName);
}

error RendererManager::BuildRenderElement(const RenderElementPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void RendererManager::DoResizingPrimaryTarget(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ResizePrimaryRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    auto target = GetPrimaryRenderTarget();
    if (!target) return;
    target->Resize(cmd->GetDimension());
}

void RendererManager::ClearAllRenderer()
{
    if (m_renderers.size() == 0) return;
    m_renderers.clear();

    m_accumulateRendererStamp = 0;
}

void RendererManager::ClearAllRenderTarget()
{
    if (m_renderTargets.size() == 0) return;
    m_renderTargets.clear();
}

void RendererManager::DoCreatingRenderer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::CreateRenderer, Frameworks::ICommand>(c);
    if (!cmd) return;
    CreateRenderer(cmd->GetRendererName());
}

void RendererManager::DoDestroyingRenderer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::DestroyRenderer, Frameworks::ICommand>(c);
    if (!cmd) return;
    DestroyRenderer(cmd->GetRendererName());
}

void RendererManager::DoCreatingRenderTarget(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::CreateRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    CreateRenderTarget(cmd->GetRenderTargetName(), cmd->GetPrimaryType());
}

void RendererManager::DoDestroyingRenderTarget(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::DestroyRenderTarget, Frameworks::ICommand>(c);
    if (!cmd) return;
    DestroyRenderTarget(cmd->GetRenderTargetName());
}

void RendererManager::DoBuildingRenderElement(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Enigma::Renderer::BuildRenderElement, Frameworks::ICommand>(c);
    if (!cmd) return;
    BuildRenderElement(cmd->GetPolicy());
}

void RendererManager::OnBuilderRenderElementBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_elementBuilder);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderElementBuilder::RenderElementBuilt, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<RenderElementBuilt>(ev->GetPolicy(), ev->GetElement()));
    m_isCurrentBuilding = false;
}

void RendererManager::OnBuildRenderElementFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderElementFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("render element (%s, %s) build failed : %s\n",
        ev->GetPolicy().m_renderBufferSignature.GetName().c_str(), ev->GetPolicy().m_effectMaterialName.c_str(),
        ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}
