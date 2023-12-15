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
}

#endif // SCENE_GRAPH_QUERIES_H
