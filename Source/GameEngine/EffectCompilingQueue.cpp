#include "EffectCompilingQueue.h"
#include "EngineErrors.h"
#include "EffectMaterial.h"
#include "EffectCompiler.h"
#include "EffectEvents.h"
#include "EffectMaterialSource.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>


using namespace Enigma::Engine;

EffectCompilingQueue::EffectCompilingQueue()
{
    m_compiler = menew EffectCompiler();
    registerHandlers();
}

EffectCompilingQueue::~EffectCompilingQueue()
{
    SAFE_DELETE(m_compiler);
    unregisterHandlers();
}

std::error_code EffectCompilingQueue::enqueue(const std::shared_ptr<EffectMaterial>& effect, const EffectCompilingProfile& profile)
{
    assert(effect);
    if (!effect->lazyStatus().isGhost()) return ErrorCode::effectAlreadyCompiled;
    std::lock_guard locker{ m_queueLock };
    m_queue.push({ effect, profile });
    effect->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::InQueue);
    return ErrorCode::ok;
}

std::error_code EffectCompilingQueue::compileNextEffect()
{
    if (m_currentCompilingEffect) return ErrorCode::ok;
    assert(m_compiler);
    std::lock_guard locker{ m_queueLock };
    if (m_queue.empty())
    {
        m_currentCompilingEffect = nullptr;
        return ErrorCode::ok;
    }
    auto& [effect, profile] = m_queue.front();
    m_currentCompilingEffect = effect;
    m_compiler->compileEffect(effect, profile);
    effect->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::Loading);
    m_queue.pop();
    return ErrorCode::ok;
}

void EffectCompilingQueue::registerHandlers()
{
    m_onCompilerEffectMaterialCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onCompilerEffectMaterialCompiled(e); });
    Frameworks::EventPublisher::subscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onCompilerEffectMaterialCompiled);
    m_onCompilerCompileEffectMaterialFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onCompilerCompileEffectMaterialFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(EffectCompiler::CompileEffectMaterialFailed), m_onCompilerCompileEffectMaterialFailed);
}

void EffectCompilingQueue::unregisterHandlers()
{
    Frameworks::EventPublisher::unsubscribe(typeid(EffectCompiler::EffectMaterialCompiled), m_onCompilerEffectMaterialCompiled);
    m_onCompilerEffectMaterialCompiled = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(EffectCompiler::CompileEffectMaterialFailed), m_onCompilerCompileEffectMaterialFailed);
    m_onCompilerCompileEffectMaterialFailed = nullptr;
}

void EffectCompilingQueue::onCompilerEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!m_currentCompilingEffect) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectCompiler::EffectMaterialCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->id() != m_currentCompilingEffect->id()) return;

    if (m_currentCompilingEffect->getEffectMaterialSource())
    {
        m_currentCompilingEffect->getEffectMaterialSource()->hydrateDuplicatedEffects();
    }
    Frameworks::EventPublisher::post(std::make_shared<EffectMaterialSourceCompiled>(m_currentCompilingEffect->id()));
    m_currentCompilingEffect = nullptr;
    auto er = compileNextEffect();
}

void EffectCompilingQueue::onCompilerCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCompilingEffect) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectCompiler::CompileEffectMaterialFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->id() != m_currentCompilingEffect->id()) return;

    Platforms::Debug::ErrorPrintf("effect material %s compile failed : %s\n", ev->id().name().c_str(), ev->error().message().c_str());
    Frameworks::EventPublisher::post(std::make_shared<CompileEffectMaterialSourceFailed>(ev->id(), ev->error()));
    m_currentCompilingEffect = nullptr;
    auto er = compileNextEffect();
}
