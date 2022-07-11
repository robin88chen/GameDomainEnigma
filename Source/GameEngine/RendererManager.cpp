#include "RendererManager.h"
#include "EngineErrors.h"
#include "RendererEvents.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, RendererManager);

RendererManager::RendererManager(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, Engine, RendererManager, ISystemService);
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
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult RendererManager::OnTerm()
{
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

error RendererManager::CreateRenderTarget(const std::string& name, RenderTarget::PrimaryType primary,
                                          Graphics::IGraphicAPI::AsyncType async)
{
    auto target_check = GetRenderTarget(name);
    if (target_check)
    {
        // render already exist
        return ErrorCode::renderTargetAlreadyExisted;
    }
    RenderTargetPtr target = RenderTargetPtr{ menew RenderTarget(name, primary, async) };
    m_mapRenderTarget.emplace(name, target);

    if (primary == RenderTarget::PrimaryType::IsPrimary)
    {
        m_primaryRenderTargetName = name;
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew PrimaryRenderTargetCreated(name) });
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

void RendererManager::ClearAllRenderTarget()
{
    if (m_mapRenderTarget.size() == 0) return;
    m_mapRenderTarget.clear();
}
