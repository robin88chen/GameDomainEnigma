/*********************************************************************
 * \file   GameLightEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef GAME_LIGHT_EVENTS_H
#define GAME_LIGHT_EVENTS_H

#include "Frameworks/Event.h"
#include "SceneGraph/Light.h"

namespace Enigma::GameCommon
{
    class GameLightCreated : public Frameworks::IEvent
    {
    public:
        GameLightCreated(const std::shared_ptr<SceneGraph::Light>& light) : m_light(light) {}

        std::shared_ptr<SceneGraph::Light> light() const { return m_light.lock(); }

    protected:
        std::weak_ptr<SceneGraph::Light> m_light;
    };

    class CreateGameLightFailed : public Frameworks::IEvent
    {
    public:
        CreateGameLightFailed(const SceneGraph::SpatialId& id, std::error_code err) : m_id(id), m_err(err) {}

        const SceneGraph::SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SceneGraph::SpatialId m_id;
        std::error_code m_err;
    };
}

#endif // GAME_LIGHT_EVENTS_H
