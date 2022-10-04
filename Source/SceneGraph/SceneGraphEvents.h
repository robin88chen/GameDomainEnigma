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
#include <memory>

namespace Enigma::SceneGraph
{
    class Spatial;

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
}


#endif // SCENE_GRAPH_EVENTS_H
