/*****************************************************************
 * \file   WorldMapRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef WORLD_MAP_REPOSITORY_H
#define WORLD_MAP_REPOSITORY_H

#include "QuadTreeRootId.h"
#include "WorldMapId.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include <memory>
#include <unordered_map>
#include <mutex>

namespace Enigma::WorldMap
{
    class QuadTreeRoot;
    class WorldMap;
    class WorldMapStoreMapper;

    class WorldMapRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldMapRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<WorldMapStoreMapper>& world_map_store_mapper);
        WorldMapRepository(const WorldMapRepository&) = delete;
        WorldMapRepository(WorldMapRepository&&) = delete;
        WorldMapRepository& operator=(const WorldMapRepository&) = delete;
        WorldMapRepository& operator=(WorldMapRepository&&) = delete;
        ~WorldMapRepository() override;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        bool hasQuadTreeRoot(const QuadTreeRootId& id);
        std::shared_ptr<QuadTreeRoot> queryQuadTreeRoot(const QuadTreeRootId& id);
        void putQuadTreeRoot(const QuadTreeRootId& id, const std::shared_ptr<QuadTreeRoot>& quad_tree_root);
        void removeQuadTreeRoot(const QuadTreeRootId& id);

        bool hasWorldMap(const WorldMapId& id);
        std::shared_ptr<WorldMap> queryWorldMap(const WorldMapId& id);
        void putWorldMap(const WorldMapId& id, const std::shared_ptr<WorldMap>& world_map);
        void removeWorldMap(const WorldMapId& id);

    protected:
        void queryQuadTreeRoot(const Frameworks::IQueryPtr q);
        void hasQuadTreeRoot(const Frameworks::IQueryPtr q);
        void requestQuadTreeRootCreation(const Frameworks::IQueryPtr q);
        void requestQuadTreeRootConstitution(const Frameworks::IQueryPtr q);
        void queryWorldMap(const Frameworks::IQueryPtr q);
        void hasWorldMap(const Frameworks::IQueryPtr q);
        void requestWorldMapCreation(const Frameworks::IQueryPtr q);
        void requestWorldMapConstitution(const Frameworks::IQueryPtr q);

        void putWorldMap(const Frameworks::ICommandPtr c);

    protected:
        Frameworks::QuerySubscriberPtr m_queryQuadTreeRoot;
        Frameworks::QuerySubscriberPtr m_hasQuadTreeRoot;
        Frameworks::QuerySubscriberPtr m_requestQuadTreeRootCreation;
        Frameworks::QuerySubscriberPtr m_requestQuadTreeRootConstitution;
        Frameworks::QuerySubscriberPtr m_queryWorldMap;
        Frameworks::QuerySubscriberPtr m_hasWorldMap;
        Frameworks::QuerySubscriberPtr m_requestWorldMapCreation;
        Frameworks::QuerySubscriberPtr m_requestWorldMapConstitution;

        Frameworks::CommandSubscriberPtr m_putWorldMap;

        std::shared_ptr<WorldMapStoreMapper> m_storeMapper;
        std::unordered_map<QuadTreeRootId, std::shared_ptr<QuadTreeRoot>, QuadTreeRootId::hash> m_quadTreeRoots;
        std::recursive_mutex m_quadTreeRootLock;
        std::unordered_map<WorldMapId, std::shared_ptr<WorldMap>, WorldMapId::hash> m_worldMaps;
        std::recursive_mutex m_worldMapLock;
    };
}

#endif // WORLD_MAP_REPOSITORY_H
