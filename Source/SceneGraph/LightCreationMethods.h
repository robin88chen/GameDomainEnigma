/*********************************************************************
 * \file   LightCreationMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef LIGHT_CREATION_METHODS_H
#define LIGHT_CREATION_METHODS_H

#include "GameEngine/GenericDto.h"
#include "SpatialId.h"

namespace Enigma::SceneGraph
{
    class Light;
    class LightInfo;

    class LightCreationMethod
    {
    public:
        static std::shared_ptr<Light> create(const SpatialId& id, const LightInfo& light_info);
        static std::shared_ptr<Light> constitute(const SpatialId& id, const Engine::GenericDto& dto);
    };
}

#endif // LIGHT_CREATION_METHODS_H
