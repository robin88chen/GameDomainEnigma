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
#include "Frameworks/EventSubscriber.h"
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
        void registerHandlers();
        void unregisterHandlers();

        void onCompilerEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void onCompilerCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);

    protected:
        EffectCompiler* m_compiler;
        std::queue<std::pair<std::shared_ptr<EffectMaterial>, EffectCompilingProfile>> m_queue;
        std::recursive_mutex m_queueLock;
        std::shared_ptr<EffectMaterial> m_currentCompilingEffect;

        Frameworks::EventSubscriberPtr m_onCompilerEffectMaterialCompiled;
        Frameworks::EventSubscriberPtr m_onCompilerCompileEffectMaterialFailed;
    };
}

#endif // EFFECT_COMPILING_QUEUE_H
