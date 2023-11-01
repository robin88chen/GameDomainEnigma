/*********************************************************************
 * \file   WorldMapService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_SERVICE_H
#define WORLD_MAP_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "SceneGraph/LazyNode.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Terrain/TerrainPawn.h"
#include "GameEngine/BoundingVolume.h"
#include <system_error>

namespace Enigma::WorldMap
{
    class WorldMap;
    class WorldMapService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldMapService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository);
        WorldMapService(const WorldMapService& other) = delete;
        WorldMapService(WorldMapService&& other) noexcept = delete;
        WorldMapService& operator=(const WorldMapService& other) = delete;
        WorldMapService& operator=(WorldMapService&& other) noexcept = delete;
        virtual ~WorldMapService() override;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        std::vector<Engine::GenericDtoCollection> serializeQuadNodeGraphs() const;
        Engine::GenericDtoCollection serializeWorldMap() const;

        void deserializeWorldMap(const Engine::GenericDtoCollection& graph);

    protected:
        void attachTerrainToWorldMap(const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform);
        std::shared_ptr<SceneGraph::Node> queryFittingNode(const Engine::BoundingVolume& bv_in_world) const;
        std::shared_ptr<SceneGraph::Node> findFittingNodeFromQuadRoot(const std::shared_ptr<SceneGraph::Node>& root, const Engine::BoundingVolume& bv_in_node) const;
        std::shared_ptr<SceneGraph::Node> findFittingQuadLeaf(const std::shared_ptr<SceneGraph::Node>& parent, const Engine::BoundingVolume& bv_in_node, int recursive_depth) const;
        std::tuple<MathLib::Box3, unsigned> locateSubTreeBoxAndIndex(const MathLib::Box3& parent_box, const MathLib::Vector3& local_pos) const;
        bool testSubTreeQuadEnvelop(const MathLib::Box3& quad_box_in_parent, const Engine::BoundingVolume& bv_in_parent) const;
        void createFittingNode(const Engine::BoundingVolume& bv_in_world);
        std::error_code tryCreateFittingNodeFromQuadRoot(const std::shared_ptr<SceneGraph::Node>& root, const Engine::BoundingVolume& bv_in_root);
        std::error_code tryCreateFittingQuadLeaf(const std::shared_ptr<SceneGraph::Node>& parent, const Engine::BoundingVolume& bv_in_node, int recursive_depth);
        void completeCreateFittingNode(const std::shared_ptr<SceneGraph::Node>& node);
        void failCreateFittingNode(std::error_code err);

        void createEmptyWorldMap(const Frameworks::ICommandPtr& c);
        void deserializeWorldMap(const Frameworks::ICommandPtr& c);
        void attachTerrain(const Frameworks::ICommandPtr& c);
        void createFittingNode(const Frameworks::ICommandPtr& c);
        void onSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void onLazyNodeInstanced(const Frameworks::IEventPtr& e);
        void queryFittingNode(const Frameworks::IQueryPtr& q) const;

    protected:
        std::weak_ptr<WorldMap> m_world;
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;

        typedef std::list<std::weak_ptr<SceneGraph::LazyNode>> QuadRootList;
        QuadRootList m_listQuadRoot;

        Frameworks::CommandSubscriberPtr m_createWorldMap;
        Frameworks::CommandSubscriberPtr m_deserializeWorldMap;
        Frameworks::CommandSubscriberPtr m_attachTerrain;
        Frameworks::CommandSubscriberPtr m_createFittingNode;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;  // check world map is created
        Frameworks::EventSubscriberPtr m_onLazyNodeInstanced;
        Frameworks::QuerySubscriberPtr m_queryFittingNode;

        Frameworks::Ruid m_createFittingNodeRuid;
    };
}

#endif // WORLD_MAP_SERVICE_H
