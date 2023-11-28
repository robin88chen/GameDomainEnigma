/*********************************************************************
 * \file   EffectMaterialManager.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_MANAGER_H
#define EFFECT_MATERIAL_MANAGER_H

#include "EffectMaterial.h"
#include "EffectMaterialPolicy.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "EffectCommands.h"
#include <unordered_map>
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    class EffectMaterialSource;
    class EffectCompiler;

    class EffectMaterialManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        EffectMaterialManager(Frameworks::ServiceManager* srv_mngr,
            const std::shared_ptr<IEffectCompilingProfileDeserializer>& effect_deserializer);
        EffectMaterialManager(const EffectMaterialManager&) = delete;
        EffectMaterialManager(EffectMaterialManager&&) = delete;
        virtual ~EffectMaterialManager() override;
        EffectMaterialManager& operator=(const EffectMaterialManager&) = delete;
        EffectMaterialManager& operator=(EffectMaterialManager&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult onInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult onTick() override;
        /// On Term
        virtual Frameworks::ServiceResult onTerm() override;

        bool HasEffectMaterial(const std::string& name);
        EffectMaterialPtr QueryEffectMaterial(const std::string& name);

        //error CompileEffectMaterial(const EffectMaterialPolicy& policy);

    private:
        void OnCompilerEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void OnCompilerCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);

        void DoCompilingEffectMaterial(const Frameworks::ICommandPtr& c);

        /** release effect material source */
        void ReleaseEffectMaterialSource(const std::shared_ptr<EffectMaterialSource>& eff_source);

        void DumpUnreleasedMaterial();

    private:
        std::shared_ptr<IEffectCompilingProfileDeserializer> m_effectDeserializer;

        Frameworks::EventSubscriberPtr m_onCompilerEffectMaterialCompiled;
        Frameworks::EventSubscriberPtr m_onCompilerCompileEffectMaterialFailed;

        Frameworks::CommandSubscriberPtr m_doCompilingEffectMaterial;

        typedef std::unordered_map<std::string, std::shared_ptr<EffectMaterialSource>> SourceMaterialMap;
        SourceMaterialMap m_sourceMaterials;
        std::recursive_mutex m_sourceMapLock;

        EffectCompiler* m_compiler;
        std::queue<std::shared_ptr<CompileEffectMaterial>> m_requests;
        Frameworks::Ruid m_currentCompilingRuid;
        std::string m_currentCompilingEffectName;
        bool m_isCurrentCompiling;
        std::mutex m_requestLock;
    };
}

#endif // EFFECT_MATERIAL_MANAGER_H
