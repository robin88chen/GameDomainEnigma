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

        void BuildModelPrimitive(std::unique_ptr<ModelPrimitivePolicy> policy);

    protected:
        void PushInnerMesh(const std::string& node_name, std::unique_ptr<MeshPrimitivePolicy>& policy);
        void ContinueBuildInnerMesh();
        void CompleteModelPrimitive();

        void OnMeshPrimitiveBuilt(const Frameworks::IEventPtr&);
        void OnBuildMeshPrimitiveFailed(const Frameworks::IEventPtr&);
    protected:
        struct MeshBuildingMeta
        {
            MeshBuildingMeta(const std::string& name, const Frameworks::Ruid& ruid) : m_nodeName(name), m_ruidPolicy(ruid) {}
            std::string m_nodeName;
            Frameworks::Ruid m_ruidPolicy;
        };
    protected:
        MeshPrimitiveBuilder* m_meshBuilder;

        std::unique_ptr<ModelPrimitivePolicy> m_policy;
        std::shared_ptr<ModelPrimitive> m_builtPrimitive;

        std::queue<std::unique_ptr<MeshPrimitivePolicy>> m_meshPolicies;
        std::vector<MeshBuildingMeta> m_meshBuildingMetas;

        Frameworks::EventSubscriberPtr m_onMeshPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildMeshPrimitiveFailed;
    };
}

#endif // _MODEL_PRIMITIVE_BUILDER_H
