/*****************************************************************
 * \file   SceneGraphFactoryDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef SCENE_GRAPH_FACTORY_DELEGATE_H
#define SCENE_GRAPH_FACTORY_DELEGATE_H

#include <functional>
#include <memory>

namespace Enigma::Engine
{
    class GenericDto;
}

namespace Enigma::SceneGraph
{
    class SpatialId;
    class Spatial;
    class Camera;
    class Light;
    class LightInfo;

    using SpatialCreator = std::function<std::shared_ptr<Spatial>(const SpatialId& id)>;
    using SpatialConstitutor = std::function<std::shared_ptr<Spatial>(const SpatialId& id, const Engine::GenericDto& dto)>;
    using LightCreator = std::function<std::shared_ptr<Light>(const SpatialId& id, const LightInfo& info)>;
}

#endif // SCENE_GRAPH_FACTORY_DELEGATE_H
