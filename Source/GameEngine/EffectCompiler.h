/*********************************************************************
 * \file   EffectCompiler.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_COMPILER_H
#define EFFECT_COMPILER_H

#include "EffectCompilingPolicies.h"
#include "EffectPass.h"
#include "Frameworks/EventSubscriber.h"
#include <unordered_map>

namespace Enigma::Engine
{
    class EffectCompiler
    {
    public:
        EffectCompiler();
        EffectCompiler(const EffectCompiler&) = delete;
        EffectCompiler(EffectCompiler&&) = delete;
        ~EffectCompiler();
        EffectCompiler& operator=(const EffectCompiler&) = delete;
        EffectCompiler& operator=(EffectCompiler&&) = delete;

        void CompileEffect(const EffectCompilingPolicy& policy);

    private:
        void OnShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void OnProgramBuildFailed(const Frameworks::IEventPtr& e);

        void TryBuildEffectPass(const std::string& name);

    private:
        struct BuiltEffectPass
        {
            std::string m_name;
            std::optional<EffectPass> m_pass;
        };
        struct BuiltEffectTechnique
        {
            std::string m_name;
            std::vector<BuiltEffectPass> m_passes;
        };
    private:
        EffectCompilingPolicy m_policy;

        Frameworks::EventSubscriberPtr m_onShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onProgramBuildFailed;

        std::unordered_map<std::string, Graphics::IShaderProgramPtr> m_builtPrograms;
        std::unordered_map<std::string, EffectPassStates> m_builtPassStates;
        std::vector<BuiltEffectTechnique> m_builtEffectTechniques;
    };
}

#endif // EFFECT_COMPILER_H
