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
        : SceneGraphEvent(parent), m_child(child), m_code(code)  {};
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

        std::shared_ptr<Light> GetLight() { return m_light.lock(); }
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
    class InPlaceSceneGraphBuilt : public Frameworks::IEvent
    {
    public:
        InPlaceSceneGraphBuilt(const std::shared_ptr<Node>& in_place_root) : m_in_placeRoot(in_place_root) {};

        const std::shared_ptr<Node>& GetInPlaceRootNode() { return m_in_placeRoot; }
    protected:
        std::shared_ptr<Node> m_in_placeRoot;
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

        const std::shared_ptr<Pawn>& GetPawn() { return m_pawn; }
    protected:
        std::shared_ptr<Pawn> m_pawn;
    };

    class BuildPawnPrimitiveFailed : public  Frameworks::IEvent
    {
    public:
        BuildPawnPrimitiveFailed(const std::shared_ptr<Pawn>& pawn, std::error_code er)
            : m_pawn(pawn), m_error(er) {}

        std::shared_ptr<Pawn> GetPawn() { return m_pawn.lock(); }
        std::error_code GetErrorCode() const { return m_error; }
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
}


#endif // SCENE_GRAPH_EVENTS_H
