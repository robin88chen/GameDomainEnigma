#include "EffectCompilingQueue.h"
#include "EngineErrors.h"
#include "EffectMaterial.h"
#include "EffectCompiler.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include <cassert>

using namespace Enigma::Engine;

EffectCompilingQueue::EffectCompilingQueue()
{
    m_compiler = menew EffectCompiler();
}

EffectCompilingQueue::~EffectCompilingQueue()
{
    SAFE_DELETE(m_compiler);
}

std::error_code EffectCompilingQueue::enqueue(const std::shared_ptr<EffectMaterial>& effect, const EffectCompilingProfile& profile)
{
    assert(effect);
    if (!effect->lazyStatus().isGhost()) return ErrorCode::effectAlreadyCompiled;
    std::lock_guard locker{ m_queueLock };
    m_queue.push({ effect, profile });
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
    m_compiler->compileEffect(profile);
    m_queue.pop();
    return ErrorCode::ok;
}
