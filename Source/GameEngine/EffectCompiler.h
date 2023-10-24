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
            const std::string& getName() { return m_name; }
            bool HasEffect() { return m_effect != nullptr; }
            std::shared_ptr<EffectMaterial> GetEffect() { return m_effect; }
            bool HasExisted() { return m_hasExisted; }
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
            const std::string& getName() { return m_name; }
            std::error_code GetErrorCode() const { return m_error; }
        private:
            std::string m_name;
            std::error_code m_error;
        };
        class CompilingProfileDeserialized : public Frameworks::IEvent
        {
        public:
            CompilingProfileDeserialized(const Frameworks::Ruid& ruid, const EffectCompilingProfile& profile) :
                m_ruid(ruid), m_profile(profile) {};
            const Frameworks::Ruid& getRuid() { return m_ruid; }
            const EffectCompilingProfile& GetProfile() { return m_profile; }
        private:
            Frameworks::Ruid m_ruid;
            EffectCompilingProfile m_profile;
        };
        class DeserializeCompilingProfileFailed : public Frameworks::IEvent
        {
        public:
            DeserializeCompilingProfileFailed(const Frameworks::Ruid& ruid, std::error_code er) :
                m_ruid(ruid), m_error(er) {};
            const Frameworks::Ruid& getRuid() { return m_ruid; }
            std::error_code GetErrorCode() const { return m_error; }
        private:
            Frameworks::Ruid m_ruid;
            std::error_code m_error;
        };

    public:
        EffectCompiler(EffectMaterialManager* host);
        EffectCompiler(const EffectCompiler&) = delete;
        EffectCompiler(EffectCompiler&&) = delete;
        ~EffectCompiler();
        EffectCompiler& operator=(const EffectCompiler&) = delete;
        EffectCompiler& operator=(EffectCompiler&&) = delete;

        CompilingProceed CompileEffectMaterial(const EffectMaterialPolicy& policy);

    private:
        void CompileEffect(const EffectCompilingProfile& profile);

        void OnCompilingProfileDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializeCompilingProfileFailed(const Frameworks::IEventPtr& e);
        void OnShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void OnBuildProgramFailed(const Frameworks::IEventPtr& e);
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
