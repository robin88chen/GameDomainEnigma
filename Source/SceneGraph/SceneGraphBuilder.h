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
#include <optional>
#include <memory>
#include <vector>

namespace Enigma::SceneGraph
{
    class Spatial;
    class SceneGraphRepository;
    class SpatialLinkageResolver;

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
            std::vector<BuiltSpatialMeta> m_builtSpatialMetas;
        };
    public:
        SceneGraphBuilder(SceneGraphRepository* host);
        SceneGraphBuilder(const SceneGraphBuilder&) = delete;
        SceneGraphBuilder(SceneGraphBuilder&&) = delete;
        ~SceneGraphBuilder();
        SceneGraphBuilder& operator=(const SceneGraphBuilder&) = delete;
        SceneGraphBuilder& operator=(SceneGraphBuilder&&) = delete;

        void BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::GenericDto>& dtos);

    private:
        void NodeFactory(const Engine::GenericDto& dto);
        void LightFactory(const Engine::GenericDto& dto);

        void OnFactoryCreated(const Frameworks::IEventPtr& e);
        void TryCompleteSceneGraphBuilding();

    private:
        SceneGraphRepository* m_host;
        SpatialLinkageResolver* m_resolver;

        Frameworks::EventSubscriberPtr m_onFactoryCreated;

        BuiltSceneGraphMeta m_builtSceneGraphMeta;
    };
}

#endif // SCENE_GRAPH_BUILDER_H
