/*********************************************************************
 * \file   NodeCreationMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef NODE_CREATION_METHODS_H
#define NODE_CREATION_METHODS_H

#include <memory>

namespace Enigma::Engine
{
    class GenericDto;
}

namespace Enigma::SceneGraph
{
    class SpatialId;
    class Node;

    class NodeCreationMethod
    {
    public:
        static std::shared_ptr<Node> create(const SpatialId& id);
        static std::shared_ptr<Node> constitute(const SpatialId& id, const Engine::GenericDto& dto);
    };
}

#endif // NODE_CREATION_METHODS_H
