/*********************************************************************
 * \file   SceneGraphEvents.h
 * \brief  scene graph events, 通知用的事件裝weak ptr, 需要傳遞物件的事件才用
 *          shared_ptr
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_EVENTS_H
#define SCENE_GRAPH_EVENTS_H

#include "Frameworks/Event.h"
#include "Spatial.h"
#include <memory>
#include <system_error>

namespace Enigma::SceneGraph
{
    class Light;
    class LazyNode;
    class Pawn;

    //------------------ Scene Graph Events -------------------
    class SceneGraphEvent : public Frameworks::IEvent
    {
    public:
        SceneGraphEvent(const SpatialId& id) : m_id(id) {};
        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    class SpatialCullModeChanged : public SceneGraphEvent
    {
    public:
        SpatialCullModeChanged(const SpatialId& id) : SceneGraphEvent(id) {};
    };
    class SpatialBoundChanged : public SceneGraphEvent
    {
    public:
        SpatialBoundChanged(const SpatialId& id) : SceneGraphEvent(id) {};
    };
    class SpatialLocationChanged : public SceneGraphEvent
    {
    public:
        SpatialLocationChanged(const SpatialId& id) : SceneGraphEvent(id) {};
    };
    class SpatialRenderStateChanged : public SceneGraphEvent
    {
    public:
        SpatialRenderStateChanged(const SpatialId& id) : SceneGraphEvent(id) {};
    };
    class SpatialVisibilityChanged : public SceneGraphEvent
    {
    public:
        SpatialVisibilityChanged(const SpatialId& id) : SceneGraphEvent(id) {};
    };
    //------------ visibility manage ------------
    class VisibilityChanged : public Frameworks::IEvent
    {
    public:
        VisibilityChanged(const SpatialId& id, bool visible)
            : m_id(id), m_isVisible(visible) {};

        const SpatialId& id() { return m_id; }
        bool isVisible() const { return m_isVisible; }

    protected:
        SpatialId m_id;
        bool m_isVisible;
    };
    //------------ creator response ------------
    class SpatialCreated : public Frameworks::IEvent
    {
    public:
        SpatialCreated(const SpatialId& id, const std::shared_ptr<Spatial>& spatial) : m_id(id), m_spatial(spatial) {}
        const SpatialId& id() const { return m_id; }
        const Frameworks::Rtti& rtti() const { return m_id.rtti(); }
        std::shared_ptr<Spatial> spatial() { return m_spatial; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Spatial> m_spatial;
    };
    class CreateSpatialFailed : public Frameworks::IEvent
    {
    public:
        CreateSpatialFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class SpatialConstituted : public Frameworks::IEvent
    {
    public:
        SpatialConstituted(const SpatialId& id, const std::shared_ptr<Spatial>& spatial, bool is_persisted) : m_id(id), m_spatial(spatial), m_isPersisted(is_persisted) {};
        const SpatialId& id() const { return m_id; }
        const Frameworks::Rtti& rtti() const { return m_id.rtti(); }
        std::shared_ptr<Spatial> spatial() { return m_spatial; }
        bool isPersisted() const { return m_isPersisted; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Spatial> m_spatial;
        bool m_isPersisted;
    };
    class ConstituteSpatialFailed : public Frameworks::IEvent
    {
    public:
        ConstituteSpatialFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    //---------------- repository operation ----------------
    class SpatialPut : public Frameworks::IEvent
    {
    public:
        SpatialPut(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }
    protected:
        SpatialId m_id;
    };
    class PutSpatialFailed : public Frameworks::IEvent
    {
    public:
        PutSpatialFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class SpatialRemoved : public Frameworks::IEvent
    {
    public:
        SpatialRemoved(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class RemoveSpatialFailed : public Frameworks::IEvent
    {
    public:
        RemoveSpatialFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class LaziedContentPut : public Frameworks::IEvent
    {
    public:
        LaziedContentPut(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }
    protected:
        SpatialId m_id;
    };
    class PutLaziedContentFailed : public Frameworks::IEvent
    {
    public:
        PutLaziedContentFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class LaziedContentRemoved : public Frameworks::IEvent
    {
    public:
        LaziedContentRemoved(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class RemoveLaziedContentFailed : public Frameworks::IEvent
    {
    public:
        RemoveLaziedContentFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class LazyNodeHydrated : public Frameworks::IEvent
    {
    public:
        LazyNodeHydrated(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class LazyNodeHydrationFailed : public Frameworks::IEvent
    {
    public:
        LazyNodeHydrationFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    //------------ node operation ------------
    class NodeChildAttached : public Frameworks::IEvent
    {
    public:
        NodeChildAttached(const SpatialId& node_id, const std::shared_ptr<Spatial>& child) : m_nodeId(node_id), m_child(child) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        std::shared_ptr<Spatial> child() const { return m_child.lock(); }

    protected:
        SpatialId m_nodeId;
        std::weak_ptr<Spatial> m_child;
    };
    class NodeChildAttachmentFailed : public Frameworks::IEvent
    {
    public:
        NodeChildAttachmentFailed(const SpatialId& node_id, const SpatialId& child_id, std::error_code er) : m_nodeId(node_id), m_childId(child_id), m_error(er) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        const SpatialId& childId() const { return m_childId; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_nodeId;
        SpatialId m_childId;
        std::error_code m_error;
    };
    class NodeChildDetached : public Frameworks::IEvent
    {
    public:
        NodeChildDetached(const SpatialId& node_id, const SpatialId& child_id) : m_nodeId(node_id), m_childId(child_id) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        const SpatialId& childId() const { return m_childId; }

    protected:
        SpatialId m_nodeId;
        SpatialId m_childId;
    };
    class NodeChildDetachmentFailed : public Frameworks::IEvent
    {
    public:
        NodeChildDetachmentFailed(const SpatialId& node_id, const SpatialId& child_id, std::error_code er) : m_nodeId(node_id), m_childId(child_id), m_error(er) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        const SpatialId& childId() const { return m_childId; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_nodeId;
        SpatialId m_childId;
        std::error_code m_error;
    };
    //--------------------------------------------------------------------------
    class SceneSpatialDeleted : public Frameworks::IEvent
    {
    public:
        SceneSpatialDeleted(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }
    protected:
        SpatialId m_id;
    };
    class DeleteSceneSpatialFailed : public Frameworks::IEvent
    {
    public:
        DeleteSceneSpatialFailed(const SpatialId& id, std::error_code er) : m_id(id), m_error(er) {}

        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_id;
        std::error_code m_error;
    };
}


#endif // SCENE_GRAPH_EVENTS_H
