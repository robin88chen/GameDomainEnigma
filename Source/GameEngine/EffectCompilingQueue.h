/*********************************************************************
 * \file   EffectCompilingQueue.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_COMPILING_QUEUE_H
#define EFFECT_COMPILING_QUEUE_H

#include "EffectCompilingProfile.h"
#include <system_error>
#include <queue>

namespace Enigma::Engine
{
    class EffectMaterial;
    class EffectCompiler;

    class EffectCompilingQueue
    {
    public:
        EffectCompilingQueue();
        ~EffectCompilingQueue();

        std::error_code enqueue(const std::shared_ptr<EffectMaterial>& effect, const EffectCompilingProfile& profile);
        std::error_code compileNextEffect();

    protected:
        EffectCompiler* m_compiler;
        std::queue<std::pair<std::shared_ptr<EffectMaterial>, EffectCompilingProfile>> m_queue;
        std::recursive_mutex m_queueLock;
        std::shared_ptr<EffectMaterial> m_currentCompilingEffect;
    };
}

#endif // EFFECT_COMPILING_QUEUE_H
