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
#include "LightInfo.h"
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
        SceneGraphEvent(const std::shared_ptr<Spatial>& spatial) : m_spatial(spatial) {};
        std::shared_ptr<Spatial> GetSpatial() { return m_spatial.lock(); }

    protected:
        std::weak_ptr<Spatial> m_spatial;
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
    class SpatialVisibilityChanged : public SceneGraphEvent
    {
    public:
        SpatialVisibilityChanged(const std::shared_ptr<Spatial>& spatial) : SceneGraphEvent(spatial) {};
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
            : SceneGraphEvent(parent), m_child(child), m_code(code) {};
        std::shared_ptr<Spatial> GetParentNode() { return m_spatial.lock(); }
        std::shared_ptr<Spatial> GetChild() { return m_child.lock(); }
        const NotifyCode GetNotifyCode() const { return m_code; }
    protected:
        std::weak_ptr<Spatial> m_child;
        NotifyCode m_code;
    };
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

    //------------------------ scene graph factory event ------------------
    class FactorySpatialCreated : public Frameworks::IEvent
    {
    public:
        FactorySpatialCreated(const Engine::GenericDto& dto, const std::shared_ptr<Spatial>& spatial)
            : m_dto(dto), m_spatial(spatial) {};

        const Engine::GenericDto& GetDto() const { return m_dto; }
        const std::shared_ptr<Spatial>& GetSpatial() { return m_spatial; }

    protected:
        Engine::GenericDto m_dto;
        std::shared_ptr<Spatial> m_spatial;
    };
    class CreateFactorySpatialFailed : public Frameworks::IEvent
    {
    public:
        CreateFactorySpatialFailed(const Engine::GenericDto& dto, std::error_code er) : m_dto(dto), m_error(er) {}

        const Engine::GenericDto& GetDto() const { return m_dto; }
        std::error_code GetErrorCode() const { return m_error; }

    protected:
        Engine::GenericDto m_dto;
        std::error_code m_error;
    };
    class FactorySceneGraphBuilt : public Frameworks::IEvent
    {
    public:
        FactorySceneGraphBuilt(const std::string& scene_graph_id, const std::vector<std::shared_ptr<Spatial>>& top_levels)
            : m_sceneGraphId(scene_graph_id), m_topLevels(top_levels) {}

        const std::string& GetSceneGraphId() { return m_sceneGraphId; }
        const std::vector<std::shared_ptr<Spatial>>& GetTopLevelSpatial() { return m_topLevels; }

    protected:
        std::string m_sceneGraphId;
        std::vector<std::shared_ptr<Spatial>> m_topLevels;
    };
    class BuildFactorySceneGraphFailed : public Frameworks::IEvent
    {
    public:
        BuildFactorySceneGraphFailed(const std::string& scene_graph_id, std::error_code er)
            : m_sceneGraphId(scene_graph_id), m_error(er) {}

        const std::string& GetSceneGraphId() { return m_sceneGraphId; }
        std::error_code GetErrorCode() const { return m_error; }

    protected:
        std::string m_sceneGraphId;
        std::error_code m_error;
    };
    class InPlaceSceneGraphBuilt : public Frameworks::IEvent
    {
    public:
        InPlaceSceneGraphBuilt(const std::shared_ptr<Node>& in_place_root) : m_in_placeRoot(in_place_root) {};

        const std::shared_ptr<Node>& GetInPlaceRootNode() { return m_in_placeRoot; }
    protected:
        std::shared_ptr<Node> m_in_placeRoot;
    };
    class BuildInPlaceSceneGraphFailed : public Frameworks::IEvent
    {
    public:
        BuildInPlaceSceneGraphFailed(const std::string& in_place_root_name, std::error_code er) : m_in_placeRootName(in_place_root_name), m_error(er) {}

        const std::string& GetInPlaceRootNodeName() { return m_in_placeRootName; }
        std::error_code GetErrorCode() const { return m_error; }

    protected:
        std::string m_in_placeRootName;
        std::error_code m_error;
    };

    class LazyNodeInstanced : public Frameworks::IEvent
    {
    public:
        LazyNodeInstanced(const std::shared_ptr<LazyNode>& node) : m_node(node) {};

        const std::shared_ptr<LazyNode>& GetNode() { return m_node; }
    protected:
        std::shared_ptr<LazyNode> m_node;
    };
    class InstanceLazyNodeFailed : public  Frameworks::IEvent
    {
    public:
        InstanceLazyNodeFailed(const std::shared_ptr<LazyNode>& node, std::error_code er)
            : m_node(node), m_error(er) {}

        std::shared_ptr<LazyNode> GetNode() { return m_node.lock(); }
        std::error_code GetErrorCode() const { return m_error; }
    protected:
        std::weak_ptr<LazyNode> m_node;
        std::error_code m_error;
    };

    class PawnPrimitiveBuilt : public Frameworks::IEvent
    {
    public:
        PawnPrimitiveBuilt(const std::shared_ptr<Pawn>& pawn) : m_pawn(pawn) {};

        const std::shared_ptr<Pawn>& pawn() { return m_pawn; }
    protected:
        std::shared_ptr<Pawn> m_pawn;
    };

    class BuildPawnPrimitiveFailed : public  Frameworks::IEvent
    {
    public:
        BuildPawnPrimitiveFailed(const std::shared_ptr<Pawn>& pawn, std::error_code er)
            : m_pawn(pawn), m_error(er) {}

        std::shared_ptr<Pawn> pawn() { return m_pawn.lock(); }
        std::error_code error() const { return m_error; }
    protected:
        std::weak_ptr<Pawn> m_pawn;
        std::error_code m_error;
    };

    //------------ visibility manage ------------
    class VisibilityChanged : public Frameworks::IEvent
    {
    public:
        VisibilityChanged(const std::shared_ptr<LazyNode>& node, bool visible)
            : m_node(node), m_isVisible(visible) {};

        std::shared_ptr<LazyNode> GetNode() { return m_node.lock(); }
        bool IsVisible() const { return m_isVisible; }

    protected:
        std::weak_ptr<LazyNode> m_node;
        bool m_isVisible;
    };
    //------------ node operation ------------
    class NodeChildAttached : public Frameworks::IEvent
    {
    public:
        NodeChildAttached(const SceneGraph::SpatialId& node_id, const std::shared_ptr<SceneGraph::Spatial>& child) : m_nodeId(node_id), m_child(child) {}

        const SceneGraph::SpatialId& nodeId() const { return m_nodeId; }
        std::shared_ptr<SceneGraph::Spatial> child() const { return m_child.lock(); }

    protected:
        SceneGraph::SpatialId m_nodeId;
        std::weak_ptr<SceneGraph::Spatial> m_child;
    };
    class AttachNodeChildFailed : public Frameworks::IEvent
    {
    public:
        AttachNodeChildFailed(const SceneGraph::SpatialId& node_id, const SceneGraph::SpatialId& child_id, std::error_code er) : m_nodeId(node_id), m_childId(child_id), m_error(er) {}

        const SceneGraph::SpatialId& nodeId() const { return m_nodeId; }
        const SceneGraph::SpatialId& childId() const { return m_childId; }
        std::error_code error() const { return m_error; }

    protected:
        SceneGraph::SpatialId m_nodeId;
        SceneGraph::SpatialId m_childId;
        std::error_code m_error;
    };
    //------------ creator response ------------
    class NodeCreated : public Frameworks::IResponseEvent
    {
    public:
        NodeCreated(const Frameworks::Ruid& request_ruid, const std::shared_ptr<Node>& node) : IResponseEvent(request_ruid), m_node(node) {};

        std::shared_ptr<Node> node() const { return m_node; }

    protected:
        std::shared_ptr<Node> m_node;
    };
    class CreateNodeFailed : public Frameworks::IResponseEvent
    {
    public:
        CreateNodeFailed(const Frameworks::Ruid& ruid, std::error_code err) : IResponseEvent(ruid), m_error(err) {}

        std::error_code error() const { return m_error; }

    protected:
        std::error_code m_error;
    };
    class SpatialCreated : public Frameworks::IEvent
    {
    public:
        SpatialCreated(const SpatialId& id, const std::shared_ptr<Spatial>& spatial) : m_id(id), m_spatial(spatial) {};
        const SpatialId& id() const { return m_id; }
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
}


#endif // SCENE_GRAPH_EVENTS_H
