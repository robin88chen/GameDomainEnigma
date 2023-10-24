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
    using error = std::error_code;

    class GameLightCreated : public Frameworks::IEvent
    {
    public:
        GameLightCreated(const std::shared_ptr<SceneGraph::Light>& light) : m_light(light) {}

        std::shared_ptr<SceneGraph::Light> GetLight() const { return m_light.lock(); }

    protected:
        std::weak_ptr<SceneGraph::Light> m_light;
    };

    class CreateGameLightFailed : public Frameworks::IEvent
    {
    public:
        CreateGameLightFailed(const std::string& name, error err) : m_name(name), m_err(err) {}

        const std::string& getName() const { return m_name; }
        error GetError() const { return m_err; }

    protected:
        std::string m_name;
        error m_err;
    };
}

#endif // GAME_LIGHT_EVENTS_H
