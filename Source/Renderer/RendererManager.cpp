#include "RendererManager.h"
#include "RendererErrors.h"
#include "RendererEvents.h"
#include "RendererCommands.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>


using namespace Enigma::Renderer;

DEFINE_RTTI(Renderer, RendererManager);

RendererManager::RendererManager(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, Renderer, RendererManager, ISystemService);
    m_needTick = false;
    m_accumulateRendererStamp = 0;
}

RendererManager::~RendererManager()
{
    assert(m_mapRenderer.empty());
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
    //todo : renderers
    //ClearAllRenderer();
    ClearAllRenderTarget();
    m_accumulateRendererStamp = 0;
    return Frameworks::ServiceResult::Complete;
}

error RendererManager::CreateRenderer(const std::string& name)
{
    //todo: renderers
    return ErrorCode::ok;
}

error RendererManager::DestroyRenderer(const std::string& name)
{
    //todo: renderers
    return ErrorCode::ok;
}

error RendererManager::CreateRenderTarget(const std::string& name, RenderTarget::PrimaryType primary)
{
    auto target_check = GetRenderTarget(name);
    if (target_check)
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, primary) };
    m_mapRenderTarget.emplace(name, target);

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
    m_mapRenderTarget.erase(name);
    return ErrorCode::ok;
}

RenderTargetPtr RendererManager::GetRenderTarget(const std::string& name) const
{
    RenderTargetMap::const_iterator iter = m_mapRenderTarget.find(name);
    if (iter == m_mapRenderTarget.end()) return nullptr;
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

void RendererManager::ClearAllRenderTarget()
{
    if (m_mapRenderTarget.size() == 0) return;
    m_mapRenderTarget.clear();
}
