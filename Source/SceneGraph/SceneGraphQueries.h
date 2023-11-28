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
    class SceneQuadTreeRoot;
    class QueryCamera : public Frameworks::Query<std::shared_ptr<Camera>>
    {
    public:
        QueryCamera(const std::string& name) : m_name(name) {}

        const std::string& cameraName() const { return m_name; }

    protected:
        std::string m_name;
    };
    class QueryNode : public Frameworks::Query<std::shared_ptr<Node>>
    {
    public:
        QueryNode(const std::string& name) : m_name(name) {}

        const std::string& nodeName() const { return m_name; }

    protected:
        std::string m_name;
    };
}

#endif // SCENE_GRAPH_QUERIES_H
