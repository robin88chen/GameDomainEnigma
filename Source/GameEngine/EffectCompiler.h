/*********************************************************************
 * \file   EffectCompiler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_COMPILER_H
#define EFFECT_COMPILER_H

#include "EffectCompilingProfile.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectMaterialId.h"
#include "Frameworks/EventSubscriber.h"
#include <unordered_map>

namespace Enigma::Engine
{
    class EffectMaterial;
    class EffectMaterialManager;

    class EffectCompiler
    {
    public:
        enum class CompilingProceed : bool
        {
            False,
            True,
        };
    public:
        class EffectMaterialCompiled : public Frameworks::IEvent
        {
        public:
            EffectMaterialCompiled(const EffectMaterialId& id, std::shared_ptr<EffectMaterial> eff) :
                m_id(id), m_effect(eff) {};
            const EffectMaterialId& id() { return m_id; }
            std::shared_ptr<EffectMaterial> effect() { return m_effect; }
        private:
            EffectMaterialId m_id;
            std::shared_ptr<EffectMaterial> m_effect;
        };
        class CompileEffectMaterialFailed : public Frameworks::IEvent
        {
        public:
            CompileEffectMaterialFailed(const EffectMaterialId& id, std::error_code er) :
                m_id(id), m_error(er) {};
            const EffectMaterialId& id() { return m_id; }
            std::error_code error() const { return m_error; }
        private:
            EffectMaterialId m_id;
            std::error_code m_error;
        };

    public:
        EffectCompiler();
        EffectCompiler(const EffectCompiler&) = delete;
        EffectCompiler(EffectCompiler&&) = delete;
        ~EffectCompiler();
        EffectCompiler& operator=(const EffectCompiler&) = delete;
        EffectCompiler& operator=(EffectCompiler&&) = delete;

        void compileEffect(const std::shared_ptr<EffectMaterial>& effect, const EffectCompilingProfile& profile);

    private:
        void onShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void onBuildProgramFailed(const Frameworks::IEventPtr& e);
        void onSamplerStateCreated(const Frameworks::IEventPtr& e);
        void onBlendStateCreated(const Frameworks::IEventPtr& e);
        void onDepthStateCreated(const Frameworks::IEventPtr& e);
        void onRasterizerStateCreated(const Frameworks::IEventPtr& e);

        void tryBuildEffectPass(const std::string& program_name);
        void tryBuildEffectTechniques(const std::string& name);
        void tryBuildEffectMaterial();

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
            bool hasAllPassBuilt();
            std::vector<EffectPass> retrieveEffectPasses();
        };
    private:
        EffectCompilingProfile m_profile;
        std::shared_ptr<EffectMaterial> m_compilingEffect;

        Frameworks::EventSubscriberPtr m_onShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onBuildProgramFailed;
        Frameworks::EventSubscriberPtr m_onSamplerStateCreated;
        Frameworks::EventSubscriberPtr m_onBlendStateCreated;
        Frameworks::EventSubscriberPtr m_onDepthStateCreated;
        Frameworks::EventSubscriberPtr m_onRasterizerStateCreated;

        std::unordered_map<std::string, Graphics::IShaderProgramPtr> m_builtPrograms;
        std::unordered_map<std::string, EffectPassStates> m_builtPassStates;
        std::vector<BuiltEffectTechniqueMeta> m_builtEffectTechniques;

        bool m_hasMaterialProduced;
    };
}

#endif // EFFECT_COMPILER_H
