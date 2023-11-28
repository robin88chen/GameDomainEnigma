/*********************************************************************
 * \file   WorldMapCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_COMMANDS_H
#define WORLD_MAP_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"
#include "Terrain/TerrainPawn.h"
#include "MathLib/Matrix4.h"
#include "GameEngine/BoundingVolume.h"

namespace Enigma::WorldMap
{
    class CreateEmptyWorldMap : public Frameworks::ICommand
    {
    public:
        CreateEmptyWorldMap(const std::string& name, const Engine::FactoryDesc& factory_desc, const std::string& portal_manager_name) : m_name(name), m_factory_desc(factory_desc), m_portal_manager_name(portal_manager_name) {}

        const std::string& name() const { return m_name; }
        const Engine::FactoryDesc& factoryDesc() const { return m_factory_desc; }
        const std::string& portalManagerName() const { return m_portal_manager_name; }

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factory_desc;
        std::string m_portal_manager_name;
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
    class CreateFittingQuadNode : public Frameworks::IRequestCommand
    {
    public:
        CreateFittingQuadNode(const Engine::BoundingVolume& bv) : m_bv(bv) {}

        const Engine::BoundingVolume& getBoundingVolume() const { return m_bv; }
    protected:
        Engine::BoundingVolume m_bv;
    };
}

#endif // WORLD_MAP_COMMANDS_H
