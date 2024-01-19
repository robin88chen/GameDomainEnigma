/*********************************************************************
 * \file   ModelPrimitiveBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_BUILDER_H
#define _MODEL_PRIMITIVE_BUILDER_H

#include "ModelPrimitive.h"
#include "RenderablePrimitivePolicies.h"
#include "RenderablePrimitiveBuilder.h"
#include "Frameworks/EventSubscriber.h"
#include "AnimatorPolicies.h"
#include <vector>

namespace Enigma::Renderables
{
    class MeshPrimitiveBuilder;

    class ModelPrimitiveBuilder
    {
    public:
        class ModelPrimitiveBuilt : public Frameworks::IEvent
        {
        public:
            ModelPrimitiveBuilt(const Frameworks::Ruid& ruid, const std::string& name, const std::shared_ptr<ModelPrimitive>& prim)
                : m_ruid(ruid), m_name(name), m_prim(prim) {};
            const Frameworks::Ruid& getRuid() const { return m_ruid; }
            const std::string& getName() const { return m_name; }
            const std::shared_ptr<ModelPrimitive>& GetPrimitive() { return m_prim; }

        private:
            Frameworks::Ruid m_ruid;
            std::string m_name;
            std::shared_ptr<ModelPrimitive> m_prim;
        };
        class BuildModelPrimitiveFailed : public Frameworks::IEvent
        {
        public:
            BuildModelPrimitiveFailed(const Frameworks::Ruid& ruid, const std::string& name, std::error_code er)
                : m_ruid(ruid), m_name(name), m_error(er) {};

            const Frameworks::Ruid& getRuid() const { return m_ruid; }
            const std::string& getName() const { return m_name; }
            std::error_code GetErrorCode() const { return m_error; }

        private:
            Frameworks::Ruid m_ruid;
            std::string m_name;
            std::error_code m_error;
        };
    public:
        ModelPrimitiveBuilder();
        ModelPrimitiveBuilder(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder(ModelPrimitiveBuilder&&) = delete;
        ~ModelPrimitiveBuilder();
        ModelPrimitiveBuilder& operator=(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder& operator=(ModelPrimitiveBuilder&&) = delete;

        void BuildModelPrimitive(const Frameworks::Ruid& ruid, const std::shared_ptr<ModelPrimitivePolicy>& policy);

    protected:
        void PushInnerMesh(const std::string& node_name, const std::shared_ptr<MeshPrimitivePolicy>& policy);
        void ContinueBuildInnerMesh();
        void TryBuildAnimator();
        void CompleteModelPrimitive();

        void OnMeshPrimitiveBuilt(const Frameworks::IEventPtr&);
        void OnBuildMeshPrimitiveFailed(const Frameworks::IEventPtr&);
        void OnModelAnimatorBuilt(const Frameworks::IEventPtr& e);
        void OnBuildModelAnimatorFailed(const Frameworks::IEventPtr& e);
    protected:
        struct MeshBuildingMeta
        {
            MeshBuildingMeta(const std::string& name, const Frameworks::Ruid& ruid) : m_nodeName(name), m_ruidPolicy(ruid) {}
            std::string m_nodeName;
            Frameworks::Ruid m_ruidPolicy;
        };
    protected:
        MeshPrimitiveBuilder* m_meshBuilder;

        Frameworks::Ruid m_buildingRuid;
        std::shared_ptr<ModelPrimitivePolicy> m_policy;
        std::shared_ptr<ModelPrimitive> m_builtPrimitive;

        std::queue<std::tuple<Frameworks::Ruid, std::shared_ptr<MeshPrimitivePolicy>>> m_meshPolicies;
        std::vector<MeshBuildingMeta> m_meshBuildingMetas;

        std::shared_ptr<ModelAnimatorPolicy> m_animatorPolicy;

        Frameworks::EventSubscriberPtr m_onMeshPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildMeshPrimitiveFailed;
        Frameworks::EventSubscriberPtr m_onModelAnimatorBuilt;
        Frameworks::EventSubscriberPtr m_onBuildModelAnimatorFailed;
    };
}

#endif // _MODEL_PRIMITIVE_BUILDER_H
