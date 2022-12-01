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
        class EffectMaterialCompiled : public Frameworks::IEvent
        {
        public:
            //! try rich event
            EffectMaterialCompiled(const std::string& filename, std::unique_ptr<EffectMaterial> eff) :
                m_filename(filename), m_effect(std::move(eff)) {};
            const std::string& GetFilename() { return m_filename; }
            bool HasEffect() { return m_effect != nullptr; }
            std::unique_ptr<EffectMaterial> GetEffect() { return std::move(m_effect); }
        private:
            std::string m_filename;
            std::unique_ptr<EffectMaterial> m_effect;
        };

    public:
        EffectCompiler(EffectMaterialManager* host);
        EffectCompiler(const EffectCompiler&) = delete;
        EffectCompiler(EffectCompiler&&) = delete;
        ~EffectCompiler();
        EffectCompiler& operator=(const EffectCompiler&) = delete;
        EffectCompiler& operator=(EffectCompiler&&) = delete;

        void CompileEffectMaterial(const EffectMaterialPolicy& policy);

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
