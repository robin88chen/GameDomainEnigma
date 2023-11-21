/*********************************************************************
 * \file   WorldMap.h
 * \brief (2023.11.21) WorldMap contains a portal zone node as root, and a list of quad tree roots
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "GameEngine/GenericDto.h"
#include "Terrain/TerrainPawn.h"
#include <string>

namespace Enigma::WorldMap
{
    class SceneQuadTreeRoot;
    class WorldMap
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        WorldMap(const std::string& name, const std::shared_ptr<SceneGraph::PortalZoneNode>& root);
        WorldMap(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const Engine::GenericDto& o);
        WorldMap(const WorldMap& other) = delete;
        WorldMap(WorldMap&& other) = delete;
        WorldMap& operator=(const WorldMap& other) = delete;
        WorldMap& operator=(WorldMap&& other) = delete;
        ~WorldMap();

        const std::string& getName() const { return m_name; };

        Engine::GenericDto serializeDto();
        std::vector<Engine::GenericDtoCollection> serializeSceneGraphs();

        std::shared_ptr<SceneGraph::PortalZoneNode> getRoot() const { return m_root.lock(); };

        void attachTerrain(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform);
        std::shared_ptr<SceneGraph::Node> queryFittingNode(const Engine::BoundingVolume& bv_in_world) const;

    protected:
        Engine::FactoryDesc m_factory_desc;
        std::string m_name;
        std::weak_ptr<SceneGraph::PortalZoneNode> m_root;
        typedef std::list<std::shared_ptr<SceneQuadTreeRoot>> QuadRootList;
        QuadRootList m_listQuadRoot;
    };
}

#endif // WORLD_MAP_H
