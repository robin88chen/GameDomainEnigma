/*********************************************************************
 * \file   WorldMap.h
 * \brief
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
    class WorldMap : public SceneGraph::PortalZoneNode
    {
        DECLARE_EN_RTTI;
    public:
        WorldMap(const std::string& name);
        WorldMap(const Engine::GenericDto& o);
        WorldMap(const WorldMap& other) = delete;
        WorldMap(WorldMap&& other) = delete;
        WorldMap& operator=(const WorldMap& other) = delete;
        WorldMap& operator=(WorldMap&& other) = delete;
        virtual ~WorldMap() override;

        const std::string& getName() const { return m_name; };

        virtual Engine::GenericDto serializeDto() override;
        virtual std::vector<Engine::GenericDtoCollection> serializeQuadGraphs();

        void setPortalRootNode(const std::shared_ptr<SceneGraph::PortalManagementNode>& root);

        std::shared_ptr<SceneGraph::PortalManagementNode> getPortalRootNode() const { return m_portalRootNode.lock(); };

        void attachTerrain(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform);
        std::shared_ptr<SceneGraph::Node> queryFittingNode(const Engine::BoundingVolume& bv_in_world) const;

    protected:
        std::weak_ptr<SceneGraph::PortalManagementNode> m_portalRootNode;
        typedef std::list<std::shared_ptr<SceneQuadTreeRoot>> QuadRootList;
        QuadRootList m_listQuadRoot;
    };
}

#endif // WORLD_MAP_H
