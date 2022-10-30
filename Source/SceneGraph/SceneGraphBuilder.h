/*********************************************************************
 * \file   SceneGraphBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_BUILDER_H
#define SCENE_GRAPH_BUILDER_H

#include "GameEngine/Contract.h"
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
            Engine::Contract m_contract;
            std::optional<std::shared_ptr<Spatial>> m_spatial;
        };
        struct BuiltSceneGraphMeta
        {
            std::string m_sceneGraphId;
            std::vector<BuiltSpatialMeta> m_builtSpatialMetas;
        };
    public:
        SceneGraphBuilder(SceneGraphRepository* host);
        ~SceneGraphBuilder();

        void BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::Contract>& contracts);

    private:
        void NodeContractFactory(const Engine::Contract& contract);

        void OnContractedCreated(const Frameworks::IEventPtr& e);
        void TryCompleteSceneGraphBuilding();

    private:
        SceneGraphRepository* m_host;
        SpatialLinkageResolver* m_resolver;

        Frameworks::EventSubscriberPtr m_onContractedCreated;

        BuiltSceneGraphMeta m_builtSceneGraphMeta;
    };
}

#endif // SCENE_GRAPH_BUILDER_H
