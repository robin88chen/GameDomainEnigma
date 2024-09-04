/*********************************************************************
 * \file   LazyNodeCreationMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef LAZY_NODE_CREATION_METHODS_H
#define LAZY_NODE_CREATION_METHODS_H

#include "GameEngine/GenericDto.h"
#include <memory>
#include <system_error>

namespace Enigma::SceneGraph
{
    class LazyNode;
    class SpatialId;
    class VisibilityManagedNode;

    class LazyNodeCreationMethod
    {
    public:
        static std::shared_ptr<LazyNode> create(const SpatialId& id);
        static std::shared_ptr<LazyNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);
        static std::error_code hydrate(const std::shared_ptr<LazyNode>& lazy_node, const Engine::GenericDto& dto);
    };
    class VisibilityManagedNodeCreationMethod
    {
    public:
        static std::shared_ptr<VisibilityManagedNode> create(const SpatialId& id);
        static std::shared_ptr<VisibilityManagedNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);
    };
}

#endif // LAZY_NODE_CREATION_METHODS_H
