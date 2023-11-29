/*********************************************************************
 * \file   SceneGraphStoreMapper.h
 * \brief  scene graph store mapper interface, used to map the store (e.g. database)
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_STORE_MAPPER_H
#define SCENE_GRAPH_STORE_MAPPER_H

#include "SpatialId.h"
#include <system_error>

namespace Enigma::SceneGraph
{
    class Camera;
    class SceneGraphStoreMapper
    {
    public:
        virtual ~SceneGraphStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasCamera(const SpatialId& id) = 0;
        virtual std::shared_ptr<Camera> queryCamera(const SpatialId& id) = 0;
        virtual std::error_code serializeCamera(const SpatialId& id, const std::shared_ptr<Camera>& camera) = 0;
    };
}

#endif // SCENE_GRAPH_STORE_MAPPER_H
