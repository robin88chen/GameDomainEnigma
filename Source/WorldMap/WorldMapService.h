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
#include "SceneGraph/LazyNode.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Terrain/TerrainPawn.h"

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

        std::vector<Engine::GenericDtoCollection> SerializeQuadNodeGraphs() const;
        Engine::GenericDtoCollection SerializeWorldMap() const;

        void DeserializeWorldMap(const Engine::GenericDtoCollection& graph);

    protected:
        void AttachTerrainToWorldMap(const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform);

        void DoCreatingEmptyWorldMap(const Frameworks::ICommandPtr& c);
        void DoDeserializingWorldMap(const Frameworks::ICommandPtr& c);
        void DoAttachingTerrain(const Frameworks::ICommandPtr& c);
        void OnSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void OnLazyNodeInstanced(const Frameworks::IEventPtr& e);

    protected:
        std::weak_ptr<WorldMap> m_world;
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;

        typedef std::list<std::weak_ptr<SceneGraph::LazyNode>> QuadRootList;
        QuadRootList m_listQuadRoot;

        Frameworks::CommandSubscriberPtr m_doCreatingWorldMap;
        Frameworks::CommandSubscriberPtr m_doDeserializingWorldMap;
        Frameworks::CommandSubscriberPtr m_doAttachingTerrain;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;  // check world map is created
        Frameworks::EventSubscriberPtr m_onLazyNodeInstanced;
    };
}

#endif // WORLD_MAP_SERVICE_H
