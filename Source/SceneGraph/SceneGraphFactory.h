/*********************************************************************
 * \file   SceneGraphFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_FACTORY_H
#define SCENE_GRAPH_FACTORY_H

#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/GenericDto.h"
#include <mutex>

namespace Enigma::SceneGraph
{
    class SpatialId;
    class Camera;
    class Pawn;

    class SceneGraphFactory
    {
    public:
        SceneGraphFactory();
        ~SceneGraphFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Camera> createCamera(const SpatialId& id);
        std::shared_ptr<Camera> constituteCamera(const SpatialId& id, const Engine::GenericDto& dto);

        std::shared_ptr<Pawn> createPawn(const SpatialId& id);
        std::shared_ptr<Pawn> constitutePawn(const SpatialId& id, const Engine::GenericDtoCollection& dtos);

    protected:
        void createCamera(const Frameworks::ICommandPtr& c);
        void constituteCamera(const Frameworks::ICommandPtr& c);
        void createPawn(const Frameworks::ICommandPtr& c);
        void constitutePawn(const Frameworks::ICommandPtr& c);

        void onPrimitiveBuilt(const Frameworks::IEventPtr& e);
        void onBuildPrimitiveFailed(const Frameworks::IEventPtr& e);

    protected:
        Frameworks::CommandSubscriberPtr m_createCamera;
        Frameworks::CommandSubscriberPtr m_constituteCamera;
        Frameworks::CommandSubscriberPtr m_createPawn;
        Frameworks::CommandSubscriberPtr m_constitutePawn;

        std::unordered_map<Frameworks::Ruid, std::shared_ptr<Pawn>, Frameworks::Ruid::HashFunc> m_buildingPawnPrimitives; // policy ruid -> pawn
        std::recursive_mutex m_buildingPrimitiveLock;

        Frameworks::EventSubscriberPtr m_onPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildPrimitiveFailed;
    };
}

#endif // SCENE_GRAPH_FACTORY_H
