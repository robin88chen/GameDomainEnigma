#include "RendererManager.h"
#include "RendererErrors.h"
#include "RendererCommands.h"
#include "Renderer.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>


using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, RendererManager);

RendererManager::RendererManager(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, Renderer, RendererManager, ISystemService);
    m_needTick = false;
    m_accumulateRendererStamp = 0;
}

RendererManager::~RendererManager()
{
    assert(m_renderers.empty());
}

Enigma::Frameworks::ServiceResult RendererManager::OnInit()
{
    m_accumulateRendererStamp = 0;
    m_doResizingPrimaryTarget =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoResizingPrimaryTarget(c); });
    Frameworks::CommandBus::Subscribe(typeid(ResizePrimaryRenderTarget), m_doResizingPrimaryTarget);
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RendererManager::OnTerm()
{
    Frameworks::CommandBus::Unsubscribe(typeid(ResizePrimaryRenderTarget), m_doResizingPrimaryTarget);
    m_doResizingPrimaryTarget = nullptr;
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

    return ErrorCode::ok;
}

error RendererManager::DestroyRenderer(const std::string& name)
{
    IRendererPtr render = GetRenderer(name);
    if (!render) return ErrorCode::rendererNotExist;

    unsigned int stamp = render->GetStampBitMask();
    m_accumulateRendererStamp &= (~stamp);
    m_renderers.erase(name);
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
