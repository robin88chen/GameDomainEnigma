/*********************************************************************
 * \file   SceneGraphEvents.h
 * \brief  scene graph events
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_EVENTS_H
#define SCENE_GRAPH_EVENTS_H

#include "Frameworks/Event.h"
#include "LightInfo.h"
#include "Spatial.h"
#include <memory>

namespace Enigma::SceneGraph
{
    class Light;

    //------------------ Scene Graph Events -------------------
    class SceneGraphEvent : public Frameworks::IEvent
    {
    public:
        SceneGraphEvent(const std::shared_ptr<Spatial>& spatial) : m_spatial(spatial) {};
        const std::shared_ptr<Spatial>& GetSpatial() { return m_spatial; }

    protected:
        std::shared_ptr<Spatial> m_spatial;
    };
    class SpatialCullModeChanged : public SceneGraphEvent
    {
    public:
        SpatialCullModeChanged(const std::shared_ptr<Spatial>& spatial) : SceneGraphEvent(spatial) {};
    };
    class SpatialBoundChanged : public SceneGraphEvent
    {
    public:
        SpatialBoundChanged(const std::shared_ptr<Spatial>& spatial) : SceneGraphEvent(spatial) {};
    };
    class SpatialLocationChanged : public SceneGraphEvent
    {
    public:
        SpatialLocationChanged(const std::shared_ptr<Spatial>& spatial) : SceneGraphEvent(spatial) {};
    };
    class SpatialRenderStateChanged : public SceneGraphEvent
    {
    public:
        SpatialRenderStateChanged(const std::shared_ptr<Spatial>& spatial) : SceneGraphEvent(spatial) {};
    };
    class SceneGraphChanged : public SceneGraphEvent
    {
    public:
        enum class NotifyCode
        {
            Invalid = 0,
            AttachChild,
            DetachChild,
            DeferredInstanced,
        };
    public:
        SceneGraphChanged(const std::shared_ptr<Spatial>& parent, const std::shared_ptr<Spatial> child, NotifyCode code)
        : SceneGraphEvent(parent), m_child(child), m_code(code)  {};
        const std::shared_ptr<Spatial>& GetParentNode() { return m_spatial; }
        const std::shared_ptr<Spatial>& GetChild() { return m_child; }
        const NotifyCode GetNotifyCode() const { return m_code; }
    protected:
        std::shared_ptr<Spatial> m_child;
        NotifyCode m_code;
    };
    //------------------------- Light Info Events --------------------------
    class LightInfoEvent : public Frameworks::IEvent
    {
    public:
        LightInfoEvent(const std::shared_ptr<Light>& lit) : m_light(lit) {};

        const std::shared_ptr<Light>& GetLight() { return m_light; }
    protected:
        std::shared_ptr<Light> m_light;
    };
    class LightInfoCreated : public LightInfoEvent
    {
    public:
        LightInfoCreated(const std::shared_ptr<Light>& lit) : LightInfoEvent(lit) {}
    };
    class LightInfoDeleted : public LightInfoEvent
    {
    public:
        LightInfoDeleted(const std::string& name, LightInfo::LightType light_type) : LightInfoEvent(nullptr),
            m_lightName{ name }, m_lightType{ light_type } {}

        const std::string& GetLightName() { return m_lightName; }
        LightInfo::LightType GetLightType() const { return m_lightType; }
    protected:
        std::string m_lightName;
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

        NotifyCode GetNotifyCode() const { return m_notifyCode; }
    protected:
        NotifyCode m_notifyCode;
    };

    //------------------------ scene graph contract factory event ------------------
    class ContractedSpatialCreated : public Frameworks::IEvent
    {
    public:
        ContractedSpatialCreated(const std::string& name, const std::shared_ptr<Spatial>& spatial)
            : m_name(name), m_spatial(spatial) {};

        const std::string& GetName() { return m_name; }
        const std::shared_ptr<Spatial>& GetSpatial() { return m_spatial; }
        const Frameworks::Rtti& GetRtti() const { return m_spatial->TypeInfo(); }

    protected:
        std::string m_name;
        std::shared_ptr<Spatial> m_spatial;
    };
}


#endif // SCENE_GRAPH_EVENTS_H
