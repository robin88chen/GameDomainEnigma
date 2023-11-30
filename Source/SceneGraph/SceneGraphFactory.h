/*********************************************************************
 * \file   SceneGraphFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_FACTORY_H
#define SCENE_GRAPH_FACTORY_H

#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::SceneGraph
{
    class SpatialId;
    class Camera;

    class SceneGraphFactory
    {
    public:
        SceneGraphFactory();
        ~SceneGraphFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Camera> createCamera(const SpatialId& id);
        std::shared_ptr<Camera> constituteCamera(const SpatialId& id, const Engine::GenericDto& dto);

    protected:
        void createCamera(const Frameworks::ICommandPtr& c);
        void constituteCamera(const Frameworks::ICommandPtr& c);

    protected:
        Frameworks::CommandSubscriberPtr m_createCamera;
        Frameworks::CommandSubscriberPtr m_constituteCamera;
    };
}

#endif // SCENE_GRAPH_FACTORY_H
