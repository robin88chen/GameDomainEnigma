/*********************************************************************
 * \file   LightingPawn.h
 * \brief  base class for light quad & light volume
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LIGHTING_PAWN_H
#define LIGHTING_PAWN_H

#include "SceneGraph/Pawn.h"
#include "SceneGraph/Light.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::Rendering
{
    using error = std::error_code;
    class LightingPawn : public SceneGraph::Pawn
    {
        DECLARE_EN_RTTI;
    public:
        LightingPawn(const SceneGraph::SpatialId& id);
        LightingPawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        LightingPawn(const LightingPawn&) = delete;
        LightingPawn(LightingPawn&&) = delete;
        virtual ~LightingPawn() override;
        LightingPawn& operator=(const LightingPawn&) = delete;
        LightingPawn& operator=(LightingPawn&&) = delete;

        virtual void registerHandlers();
        virtual void unregisterHandlers();

        void setHostLight(const std::shared_ptr<SceneGraph::Light>& light);
        std::shared_ptr<SceneGraph::Light> getHostLight() const { return m_hostLight.expired() ? nullptr : m_hostLight.lock(); }

    protected:
        virtual void onLightInfoUpdated(const Frameworks::IEventPtr& e) = 0;

    protected:
        std::weak_ptr<SceneGraph::Light> m_hostLight;
        Frameworks::EventSubscriberPtr m_onLightInfoUpdated;
    };
}

#endif // LIGHTING_PAWN_H
