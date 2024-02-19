/*********************************************************************
 * \file   SceneGraphQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_QUERIES_H
#define SCENE_GRAPH_QUERIES_H

#include "Frameworks/Query.h"
#include "SceneGraphPersistenceLevel.h"

namespace Enigma::SceneGraph
{
    class Camera;
    class Node;
    class Pawn;
    class SceneQuadTreeRoot;
    class QueryCamera : public Frameworks::Query<std::shared_ptr<Camera>>
    {
    public:
        QueryCamera(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class RequestCameraCreation : public Frameworks::Query<std::shared_ptr<Camera>>
    {
    public:
        RequestCameraCreation(const SpatialId& id, PersistenceLevel persistence_level) : m_id(id), m_persistenceLevel(persistence_level) {}

        const SpatialId& id() { return m_id; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        SpatialId m_id;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestCameraConstitution : public Frameworks::Query<std::shared_ptr<Camera>>
    {
    public:
        RequestCameraConstitution(const SpatialId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}

        const SpatialId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        SpatialId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
    class QueryNode : public Frameworks::Query<std::shared_ptr<Node>>
    {
    public:
        QueryNode(const std::string& name) : m_name(name) {}

        const std::string& nodeName() const { return m_name; }

    protected:
        std::string m_name;
    };
    class QuerySpatial : public Frameworks::Query<std::shared_ptr<Spatial>>
    {
    public:
        QuerySpatial(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class RequestSpatialCreation : public Frameworks::Query<std::shared_ptr<Spatial>>
    {
    public:
        RequestSpatialCreation(const SpatialId& id, PersistenceLevel persistence_level) : m_id(id), m_persistenceLevel(persistence_level) {}

        const SpatialId& id() { return m_id; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        SpatialId m_id;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestSpatialConstitution : public Frameworks::Query<std::shared_ptr<Spatial>>
    {
    public:
        RequestSpatialConstitution(const SpatialId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}

        const SpatialId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        SpatialId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
}

#endif // SCENE_GRAPH_QUERIES_H
