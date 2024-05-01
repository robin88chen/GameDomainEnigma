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
#include "LightInfo.h"
#include "SpatialRenderState.h"

namespace Enigma::SceneGraph
{
    class Camera;
    class Node;
    class Pawn;
    class SceneQuadTreeRoot;
    class Light;
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
    class QuerySpatial : public Frameworks::Query<std::shared_ptr<Spatial>>
    {
    public:
        QuerySpatial(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class HasSpatial : public Frameworks::Query<bool>
    {
    public:
        HasSpatial(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class QueryWorldTransform : public Frameworks::Query<MathLib::Matrix4>
    {
    public:
        QueryWorldTransform(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class QueryModelBound : public Frameworks::Query<Engine::BoundingVolume>
    {
    public:
        QueryModelBound(const SpatialId& id) : m_id(id) {}

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
    class RequestLightCreation : public Frameworks::Query<std::shared_ptr<Light>>
    {
    public:
        RequestLightCreation(const SpatialId& id, const LightInfo& info, PersistenceLevel persistence_level) : m_id(id), m_info(info), m_persistenceLevel(persistence_level) {}

        const SpatialId& id() { return m_id; }
        const LightInfo& info() { return m_info; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    protected:
        SpatialId m_id;
        LightInfo m_info;
        PersistenceLevel m_persistenceLevel;
    };
    class QueryLightingStateAt : public Frameworks::Query<SpatialRenderState>
    {
    public:
        QueryLightingStateAt(const MathLib::Vector3& world_position) : m_worldPosition(world_position) {}

        const MathLib::Vector3& worldPosition() const { return m_worldPosition; }
    protected:
        MathLib::Vector3 m_worldPosition;
    };
}

#endif // SCENE_GRAPH_QUERIES_H
