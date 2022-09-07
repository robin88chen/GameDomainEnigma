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
#include "EffectTechnique.h"
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
        void OnSamplerStateCreated(const Frameworks::IEventPtr& e);
        void OnBlendStateCreated(const Frameworks::IEventPtr& e);
        void OnDepthStateCreated(const Frameworks::IEventPtr& e);
        void OnRasterizerStateCreated(const Frameworks::IEventPtr& e);

        void TryBuildEffectPass(const std::string& program_name);
        void TryBuildEffectTechniques(const std::string& name);
        void TryBuildEffectMaterial();

    private:
        struct BuiltEffectPassMeta
        {
            std::string m_name;
            std::optional<EffectPass> m_pass;
        };
        struct BuiltEffectTechniqueMeta
        {
            std::string m_name;
            std::vector<BuiltEffectPassMeta> m_passes;
            std::optional<EffectTechnique> m_technique;
            bool HasAllPassBuilt();
            std::vector<EffectPass> RetrieveEffectPasses();
        };
    private:
        EffectCompilingPolicy m_policy;

        Frameworks::EventSubscriberPtr m_onShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onProgramBuildFailed;
        Frameworks::EventSubscriberPtr m_onSamplerStateCreated;
        Frameworks::EventSubscriberPtr m_onBlendStateCreated;
        Frameworks::EventSubscriberPtr m_onDepthStateCreated;
        Frameworks::EventSubscriberPtr m_onRasterizerStateCreated;

        std::unordered_map<std::string, Graphics::IShaderProgramPtr> m_builtPrograms;
        std::unordered_map<std::string, EffectPassStates> m_builtPassStates;
        std::vector<BuiltEffectTechniqueMeta> m_builtEffectTechniques;
    };
}

#endif // EFFECT_COMPILER_H
