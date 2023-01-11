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
#include "Animators/AnimatorPolicies.h"
#include <vector>

namespace Enigma::Renderer
{
    class MeshPrimitiveBuilder;

    class ModelPrimitiveBuilder
    {
    public:
        ModelPrimitiveBuilder();
        ModelPrimitiveBuilder(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder(ModelPrimitiveBuilder&&) = delete;
        ~ModelPrimitiveBuilder();
        ModelPrimitiveBuilder& operator=(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder& operator=(ModelPrimitiveBuilder&&) = delete;

        void BuildModelPrimitive(const std::shared_ptr<ModelPrimitivePolicy>& policy);

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

        std::shared_ptr<ModelPrimitivePolicy> m_policy;
        std::shared_ptr<ModelPrimitive> m_builtPrimitive;

        std::queue<std::shared_ptr<MeshPrimitivePolicy>> m_meshPolicies;
        std::vector<MeshBuildingMeta> m_meshBuildingMetas;

        std::shared_ptr<Animators::ModelAnimatorPolicy> m_animatorPolicy;

        Frameworks::EventSubscriberPtr m_onMeshPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildMeshPrimitiveFailed;
        Frameworks::EventSubscriberPtr m_onModelAnimatorBuilt;
        Frameworks::EventSubscriberPtr m_onBuildModelAnimatorFailed;
    };
}

#endif // _MODEL_PRIMITIVE_BUILDER_H
