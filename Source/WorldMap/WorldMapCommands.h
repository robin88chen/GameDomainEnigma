/*********************************************************************
 * \file   WorldMapCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_COMMANDS_H
#define WORLD_MAP_COMMANDS_H

#include "WorldMapId.h"
#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"
#include "Terrain/TerrainPawn.h"
#include "MathLib/Matrix4.h"
#include "GameEngine/BoundingVolume.h"
#include "WorldMap.h"

namespace Enigma::WorldMap
{
    class CreateEmptyWorldMap : public Frameworks::ICommand
    {
    public:
        CreateEmptyWorldMap(const WorldMapId& id, const Engine::FactoryDesc& factory_desc, const std::optional<SceneGraph::SpatialId>& outside_region_id) : m_id(id), m_factory_desc(factory_desc), m_outside_region_id(outside_region_id) {}

        const WorldMapId& id() const { return m_id; }
        const Engine::FactoryDesc& factoryDesc() const { return m_factory_desc; }
        const std::optional<SceneGraph::SpatialId>& outsideRegionId() const { return m_outside_region_id; }

        //! ADR : 沒有合適的地方放 handler and subscriber, 改為在這裡實作
        void execute() override;

    protected:
        WorldMapId m_id;
        Engine::FactoryDesc m_factory_desc;
        std::optional<SceneGraph::SpatialId> m_outside_region_id;
    };
    class CreateWorldMapOutsideRegion : public Frameworks::ICommand
    {
    public:
        CreateWorldMapOutsideRegion(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc) : m_id(id), m_factory_desc(factory_desc) {}

        const SceneGraph::SpatialId& id() const { return m_id; }
        const Engine::FactoryDesc& factoryDesc() const { return m_factory_desc; }

        //! ADR : 沒有合適的地方放 handler and subscriber, 改為在這裡實作
        void execute() override;

    protected:
        SceneGraph::SpatialId m_id;
        Engine::FactoryDesc m_factory_desc;
    };
    class PutWorldMap : public Frameworks::ICommand
    {
    public:
        PutWorldMap(const WorldMapId& id, const std::shared_ptr<WorldMap>& world_map) : m_id(id), m_worldMap(world_map) {}

        const WorldMapId& id() const { return m_id; }
        const std::shared_ptr<WorldMap>& worldMap() const { return m_worldMap; }

    protected:
        WorldMapId m_id;
        std::shared_ptr<WorldMap> m_worldMap;
    };
    class DeserializeWorldMap : public Frameworks::ICommand
    {
    public:
        DeserializeWorldMap(const std::string& name, const Engine::GenericDtoCollection& graph, const std::string& portal_manager_name) : m_name(name), m_graph(graph), m_portal_manager_name(portal_manager_name) {}

        const std::string& name() const { return m_name; }
        const Engine::GenericDtoCollection& graph() const { return m_graph; }
        const std::string& portalManagerName() const { return m_portal_manager_name; }

    protected:
        std::string m_name;
        Engine::GenericDtoCollection m_graph;
        std::string m_portal_manager_name;
    };
    class AttachTerrainToWorldMap : public Frameworks::ICommand
    {
    public:
        AttachTerrainToWorldMap(const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform) : m_terrain(terrain), m_local(local_transform) {}

        const std::shared_ptr<Terrain::TerrainPawn>& getTerrain() const { return m_terrain; }
        const MathLib::Matrix4& getLocalTransform() const { return m_local; }

    protected:
        std::shared_ptr<Terrain::TerrainPawn> m_terrain;
        MathLib::Matrix4 m_local;
    };
}

#endif // WORLD_MAP_COMMANDS_H
