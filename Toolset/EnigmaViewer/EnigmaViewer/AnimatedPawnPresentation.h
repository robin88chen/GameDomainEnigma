/*****************************************************************
 * \file   AnimatedPawnPresentation.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef ANIMATED_PAWN_PRESENTATION_H
#define ANIMATED_PAWN_PRESENTATION_H

#include "SceneGraph/SpatialId.h"
#include "Primitives/PrimitiveId.h"
#include "GameCommon/AnimatedPawn.h"
#include "Frameworks/EventSubscriber.h"

namespace EnigmaViewer
{
    class AnimatedPawnPresentation
    {
    public:
        AnimatedPawnPresentation();
        AnimatedPawnPresentation(const AnimatedPawnPresentation&) = delete;
        AnimatedPawnPresentation(AnimatedPawnPresentation&&) = delete;
        ~AnimatedPawnPresentation();
        AnimatedPawnPresentation& operator=(const AnimatedPawnPresentation&) = delete;
        AnimatedPawnPresentation& operator=(AnimatedPawnPresentation&&) = delete;

        void subscribeHandlers();
        void unsubscribeHandlers();

        void presentPawn(const Enigma::SceneGraph::SpatialId& pawn_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::SceneGraph::SpatialId& scene_root_id);
        void removePawn(const Enigma::SceneGraph::SpatialId& scene_root_id);

        bool hasPawn() const { return m_pawn != nullptr; }

    protected:
        void assemblePawn();
        void onSpatialRemoved(const Enigma::Frameworks::IEventPtr& e);

    protected:
        Enigma::SceneGraph::SpatialId m_presentingPawnId;
        Enigma::SceneGraph::SpatialId m_removingPawnId;
        std::shared_ptr<Enigma::GameCommon::AnimatedPawn> m_pawn;
        std::optional<Enigma::Primitives::PrimitiveId> m_presentingModelId;
        Enigma::SceneGraph::SpatialId m_sceneRootId;

        Enigma::Frameworks::EventSubscriberPtr m_onSpatialRemoved;
    };
}

#endif // ANIMATED_PAWN_PRESENTATION_H
