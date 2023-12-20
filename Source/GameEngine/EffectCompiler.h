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
#include "EffectMaterialPolicy.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/ruid.h"
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
            EffectMaterialCompiled(const std::string& name, std::shared_ptr<EffectMaterial> eff, bool has_existed) :
                m_name(name), m_effect(eff), m_hasExisted(has_existed) {};
            const std::string& name() { return m_name; }
            bool hasEffect() { return m_effect != nullptr; }
            std::shared_ptr<EffectMaterial> effect() { return m_effect; }
            bool hasExisted() { return m_hasExisted; }
        private:
            std::string m_name;
            std::shared_ptr<EffectMaterial> m_effect;
            bool m_hasExisted;
        };
        class CompileEffectMaterialFailed : public Frameworks::IEvent
        {
        public:
            CompileEffectMaterialFailed(const std::string& name, std::error_code er) :
                m_name(name), m_error(er) {};
            const std::string& name() { return m_name; }
            std::error_code error() const { return m_error; }
        private:
            std::string m_name;
            std::error_code m_error;
        };
        class CompilingProfileDeserialized : public Frameworks::IEvent
        {
        public:
            CompilingProfileDeserialized(const Frameworks::Ruid& ruid, const EffectCompilingProfile& profile) :
                m_ruid(ruid), m_profile(profile) {};
            const Frameworks::Ruid& ruid() { return m_ruid; }
            const EffectCompilingProfile& profile() { return m_profile; }
        private:
            Frameworks::Ruid m_ruid;
            EffectCompilingProfile m_profile;
        };
        class DeserializeCompilingProfileFailed : public Frameworks::IEvent
        {
        public:
            DeserializeCompilingProfileFailed(const Frameworks::Ruid& ruid, std::error_code er) :
                m_ruid(ruid), m_error(er) {};
            const Frameworks::Ruid& ruid() { return m_ruid; }
            std::error_code error() const { return m_error; }
        private:
            Frameworks::Ruid m_ruid;
            std::error_code m_error;
        };

    public:
        EffectCompiler();
        EffectCompiler(const EffectCompiler&) = delete;
        EffectCompiler(EffectCompiler&&) = delete;
        ~EffectCompiler();
        EffectCompiler& operator=(const EffectCompiler&) = delete;
        EffectCompiler& operator=(EffectCompiler&&) = delete;

        CompilingProceed compileEffectMaterial(const EffectMaterialPolicy& policy);
        void compileEffect(const EffectCompilingProfile& profile);

    private:

        void onCompilingProfileDeserialized(const Frameworks::IEventPtr& e);
        void onDeserializeCompilingProfileFailed(const Frameworks::IEventPtr& e);
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
        EffectMaterialManager* m_hostManager;
        EffectCompilingProfile m_profile;

        Frameworks::Ruid m_ruidDeserializing;
        EffectMaterialPolicy m_policy;

        Frameworks::EventSubscriberPtr m_onCompilingProfileDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeCompilingProfileFailed;
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
