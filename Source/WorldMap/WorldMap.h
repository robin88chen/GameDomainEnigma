/*********************************************************************
 * \file   WorldMap.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_H
#define WORLD_MAP_H
#include <string>

#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::WorldMap
{
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

        void setPortalRootNode(const std::shared_ptr<SceneGraph::PortalManagementNode>& root);

        std::shared_ptr<SceneGraph::PortalManagementNode> getPortalRootNode() const { return m_portalRootNode.lock(); };

    protected:
        std::weak_ptr<SceneGraph::PortalManagementNode> m_portalRootNode;
    };
}

#endif // WORLD_MAP_H
