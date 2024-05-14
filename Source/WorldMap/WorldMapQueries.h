/*********************************************************************
 * \file   WorldMapQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef WORLD_MAP_QUERIES_H
#define WORLD_MAP_QUERIES_H

#include "Frameworks/Query.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/Node.h"
#include "QuadTreeRootId.h"
#include "WorldMapId.h"
#include "WorldMapPersistenceLevel.h"
#include <memory>

namespace Enigma::WorldMap
{
    class WorldMap;
    class QuadTreeRoot;
    //-------------- world map repository operations --------------
    class QueryWorldMap : public Frameworks::Query<std::shared_ptr<WorldMap>>
    {
    public:
        QueryWorldMap(const WorldMapId& id) : m_id(id) {}

        const WorldMapId& id() const { return m_id; }

    protected:
        WorldMapId m_id;
    };
    class HasWorldMap : public Frameworks::Query<bool>
    {
    public:
        HasWorldMap(const WorldMapId& id) : m_id(id) {}

        const WorldMapId& id() const { return m_id; }

    protected:
        WorldMapId m_id;
    };
    class RequestWorldMapCreation : public Frameworks::Query<std::shared_ptr<WorldMap>>
    {
    public:
        RequestWorldMapCreation(const WorldMapId& id, PersistenceLevel persistence_level) : m_id(id), m_persistenceLevel(persistence_level) {}

        const WorldMapId& id() const { return m_id; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        WorldMapId m_id;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestWorldMapConstitution : public Frameworks::Query<std::shared_ptr<WorldMap>>
    {
    public:
        RequestWorldMapConstitution(const WorldMapId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}

        const WorldMapId& id() const { return m_id; }
        const Engine::GenericDto& dto() const { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        WorldMapId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
    //-------------- quad tree root repository operations --------------
    class QueryQuadTreeRoot : public Frameworks::Query<std::shared_ptr<QuadTreeRoot>>
    {
    public:
        QueryQuadTreeRoot(const QuadTreeRootId& id) : m_id(id) {}

        const QuadTreeRootId& id() const { return m_id; }

    protected:
        QuadTreeRootId m_id;
    };
    class HasQuadTreeRoot : public Frameworks::Query<bool>
    {
    public:
        HasQuadTreeRoot(const QuadTreeRootId& id) : m_id(id) {}

        const QuadTreeRootId& id() const { return m_id; }

    protected:
        QuadTreeRootId m_id;
    };
    class RequestQuadTreeRootCreation : public Frameworks::Query<std::shared_ptr<QuadTreeRoot>>
    {
    public:
        RequestQuadTreeRootCreation(const QuadTreeRootId& id, PersistenceLevel persistence_level) : m_id(id), m_persistenceLevel(persistence_level) {}

        const QuadTreeRootId& id() const { return m_id; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        QuadTreeRootId m_id;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestQuadTreeRootConstitution : public Frameworks::Query<std::shared_ptr<QuadTreeRoot>>
    {
    public:
        RequestQuadTreeRootConstitution(const QuadTreeRootId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}

        const QuadTreeRootId& id() const { return m_id; }
        const Engine::GenericDto& dto() const { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        QuadTreeRootId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
    //-------------- fitting node operations --------------
    class QueryFittingNode : public Frameworks::Query<std::shared_ptr<SceneGraph::Node>>
    {
    public:
        QueryFittingNode(const Engine::BoundingVolume& bv) : m_bv(bv) {}

        const Engine::BoundingVolume& getBoundingVolume() const { return m_bv; }

    private:
        Engine::BoundingVolume m_bv;
    };
}

#endif // WORLD_MAP_QUERIES_H
