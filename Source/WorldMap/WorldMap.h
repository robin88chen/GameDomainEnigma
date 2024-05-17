/*********************************************************************
 * \file   WorldMap.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "WorldMapId.h"
#include "QuadTreeRootId.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraph/LazyNode.h"
#include <vector>

namespace Enigma::WorldMap
{
    class QuadTreeRoot;
    class WorldMap
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        WorldMap(const WorldMapId& id);
        WorldMap(const WorldMapId& id, const SceneGraph::SpatialId& out_region_id, const std::vector<QuadTreeRootId>& quad_roots);
        WorldMap(const WorldMapId& id, const Engine::GenericDto& dto);
        ~WorldMap();
        WorldMap(const WorldMap&) = delete;
        WorldMap(WorldMap&&) = delete;
        WorldMap& operator=(const WorldMap&) = delete;
        WorldMap& operator=(WorldMap&&) = delete;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& fd) { m_factoryDesc = fd; }
        const WorldMapId& id() const { return m_id; }

        Engine::GenericDto serializeDto() const;

        std::shared_ptr<SceneGraph::LazyNode> findFittingNode(const Engine::BoundingVolume& bv_in_world);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        WorldMapId m_id;
        SceneGraph::SpatialId m_outRegionId;
        std::vector<QuadTreeRootId> m_quadRootIds;
        std::vector<std::weak_ptr<QuadTreeRoot>> m_quadRoots;
    };
}

#endif // WORLD_MAP_H
