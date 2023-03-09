/*********************************************************************
 * \file   SceneGraphBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_BUILDER_H
#define SCENE_GRAPH_BUILDER_H

#include "GameEngine/GenericDto.h"
#include "Frameworks/EventSubscriber.h"
#include "Renderer/RenderablePrimitivePolicies.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/ResponseSubscriber.h"
#include <optional>
#include <memory>
#include <vector>
#include <deque>

namespace Enigma::SceneGraph
{
    class Spatial;
    class SceneGraphRepository;
    class SpatialLinkageResolver;
    class Pawn;
    class Node;

    class SceneGraphBuilder
    {
    private:
        struct BuiltSpatialMeta
        {
            Engine::GenericDto m_dto;
            std::optional<std::shared_ptr<Spatial>> m_spatial;
        };
        struct BuiltSceneGraphMeta
        {
            std::string m_sceneGraphId;
            std::shared_ptr<Node> m_in_placeRoot;
            std::vector<BuiltSpatialMeta> m_builtSpatialMetas;
            void Reset()
            {
                m_sceneGraphId.clear();
                m_in_placeRoot = nullptr;
                m_builtSpatialMetas.clear();
            }
        };
    public:
        SceneGraphBuilder(SceneGraphRepository* host, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
            const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer);
        SceneGraphBuilder(const SceneGraphBuilder&) = delete;
        SceneGraphBuilder(SceneGraphBuilder&&) = delete;
        ~SceneGraphBuilder();
        SceneGraphBuilder& operator=(const SceneGraphBuilder&) = delete;
        SceneGraphBuilder& operator=(SceneGraphBuilder&&) = delete;

    private:
        void DoBuildingSceneGraph(const Frameworks::ICommandPtr& c);
        void BuildNextSceneGraph();
        void BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::GenericDto>& dtos);
        void InPlaceBuildSceneGraph(const std::shared_ptr<Node>& sub_root, const std::vector<Engine::GenericDto>& dtos);

        void NodeFactory(const Engine::GenericDto& dto);
        void LazyNodeFactory(const Engine::GenericDto& dto);
        void VisibilityManagedNodeFactory(const Engine::GenericDto& dto);
        void LightFactory(const Engine::GenericDto& dto);
        void PawnFactory(const Engine::GenericDto& dto);
        void PortalZoneNodeFactory(const Engine::GenericDto& dto);
        void PortalFactory(const Engine::GenericDto& dto);
        void PortalManagementNodeFactory(const Engine::GenericDto& dto);

        void OnFactoryCreated(const Frameworks::IEventPtr& e);
        void TryCompleteSceneGraphBuilding();

        std::shared_ptr<Renderer::RenderablePrimitivePolicy> ConvertPrimitivePolicy(const Engine::GenericDto& primitive_dto);
        void BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<Renderer::RenderablePrimitivePolicy>& primitive_policy);

        void OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& r);

    private:
        SceneGraphRepository* m_host;
        SpatialLinkageResolver* m_resolver;

        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer> m_effectDeserializer;

        Frameworks::EventSubscriberPtr m_onFactoryCreated;

        Frameworks::CommandSubscriberPtr m_doBuildingSceneGraph;
        Frameworks::CommandSubscriberPtr m_doInPlaceBuildingSceneGraph;

        BuiltSceneGraphMeta m_builtSceneGraphMeta;

        std::unordered_map<Frameworks::Ruid, std::string, Frameworks::Ruid::HashFunc> m_buildingPawnPrimitives; // policy ruid -> pawn name
        std::recursive_mutex m_buildingPrimitiveLock;

        Frameworks::ResponseSubscriberPtr m_onBuildPrimitiveResponse;

        std::deque<Frameworks::ICommandPtr> m_buildCommands;
        std::recursive_mutex m_buildCommandsLock;
        std::atomic_bool m_isCurrentBuilding;
    };
}

#endif // SCENE_GRAPH_BUILDER_H
