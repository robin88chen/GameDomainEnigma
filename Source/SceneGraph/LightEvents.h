/*****************************************************************
 * \file   LightEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef LIGHT_EVENTS_H
#define LIGHT_EVENTS_H

#include "Frameworks/Event.h"
#include "LightInfo.h"
#include "SpatialId.h"
#include <system_error>

namespace Enigma::SceneGraph
{
    class Light;
    //------------------------- Light Info Events --------------------------
    class LightInfoEvent : public Frameworks::IEvent
    {
    public:
        LightInfoEvent(const std::shared_ptr<Light>& lit) : m_light(lit) {};

        std::shared_ptr<Light> light() { return m_light.lock(); }
    protected:
        std::weak_ptr<Light> m_light;
    };
    class LightInfoCreated : public LightInfoEvent
    {
    public:
        LightInfoCreated(const std::shared_ptr<Light>& lit) : LightInfoEvent(lit) {}
    };
    class LightInfoDeleted : public LightInfoEvent
    {
    public:
        LightInfoDeleted(const SpatialId& id, LightInfo::LightType light_type) : LightInfoEvent(nullptr),
            m_lightId(id), m_lightType{ light_type } {}

        const SpatialId& lightId() { return m_lightId; }
        LightInfo::LightType lightType() const { return m_lightType; }

    protected:
        SpatialId m_lightId;
        LightInfo::LightType m_lightType;
    };
    class LightInfoUpdated : public LightInfoEvent
    {
    public:
        enum class NotifyCode
        {
            Color,
            Position,
            Direction,
            Range,
            Attenuation,
            Enable,
        };
    public:
        LightInfoUpdated(const std::shared_ptr<Light>& lit, NotifyCode code) : LightInfoEvent(lit),
            m_notifyCode{ code } {}

        NotifyCode notifyCode() const { return m_notifyCode; }

    protected:
        NotifyCode m_notifyCode;
    };
}

#endif // LIGHT_EVENTS_H
