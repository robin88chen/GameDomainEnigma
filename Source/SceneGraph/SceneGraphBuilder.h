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

    using SpatialDtoFactory = std::function<Spatial* (const Engine::GenericDto& dto)>;

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
        SceneGraphBuilder(SceneGraphRepository* host, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer);
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

        void SpatialFactory(const Engine::GenericDto& dto);

        void NodeFactory(const Engine::GenericDto& dto);
        void LightFactory(const Engine::GenericDto& dto);
        void PawnFactory(const Engine::GenericDto& dto);
        void PortalFactory(const Engine::GenericDto& dto);

        void OnFactoryCreated(const Frameworks::IEventPtr& e);
        void TryCompleteSceneGraphBuilding();

        std::shared_ptr<Renderer::RenderablePrimitivePolicy> ConvertPrimitivePolicy(const std::shared_ptr<Pawn>& pawn,
            const Engine::GenericDto& primitive_dto);
        void BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<Renderer::RenderablePrimitivePolicy>& primitive_policy);

        void OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& r);

        void DoRegisteringSpatialFactory(const Frameworks::ICommandPtr& c);
        void DoUnRegisteringSpatialFactory(const Frameworks::ICommandPtr& c);
        void DoInvokingSpatialFactory(const Frameworks::ICommandPtr& c);

    private:
        SceneGraphRepository* m_host;
        SpatialLinkageResolver* m_resolver;

        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;

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

        using DtoFactoryTable = std::unordered_map<std::string, SpatialDtoFactory>;
        DtoFactoryTable m_factories;
        Frameworks::CommandSubscriberPtr m_doRegisteringSpatialFactory;
        Frameworks::CommandSubscriberPtr m_doUnRegisteringSpatialFactory;
        Frameworks::CommandSubscriberPtr m_doInvokingSpatialDtoFactory;
    };
}

#endif // SCENE_GRAPH_BUILDER_H
