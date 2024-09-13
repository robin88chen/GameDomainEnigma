﻿/*********************************************************************
 * \file   Spatial.h
 * \brief  Scene Graph Spatial, entity, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SPATIAL_H
#define SPATIAL_H

#include "SpatialId.h"
#include "MathLib/Matrix4.h"
#include "MathLib/Matrix3.h"
#include "MathLib/Quaternion.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/IRenderer.h"
#include "SceneTraveler.h"
#include "SpatialRenderState.h"
#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"
#include "SceneGraphPersistenceLevel.h"
#include <memory>
#include <system_error>
#include <bitset>

namespace Enigma::SceneGraph
{
    using error = std::error_code;
    class Culler;
    class Node;
    class SpatialAssembler;
    class SpatialDisassembler;

    /** Scene Graph Spatial Object */
    class Spatial : public std::enable_shared_from_this<Spatial>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        enum class CullingMode
        {
            Dynamic = 0,
            Always,
            Never
        };

        // flag 用途的 enum, 不要寫成 scoped enum, 運算結果會在 scope 之外
        enum SpatialBit
        {
            Spatial_None = 0x00,
            //Spatial_BelongToParent = 0x01,  ///< parent負責刪除, ==> 沒什麼用處, 刪掉
            Spatial_Hide = 0x02,
            Spatial_Unlit = 0x04,  ///< 不受光的物件, 可以不用計算lighting render state
            //Spatial_ShadowCaster = 0x10,  // move into shadow map module
            //Spatial_ShadowReceiver = 0x20, // move into shadow map module
            //Spatial_ReflectionPlane = 0x??,
            //Spatial_LightVolume = 0x??,
            Spatial_FullOpen = 0xff,
        };
        using SpatialFlags = std::bitset<16>;

        enum NotifyBit  ///< 是否需要送出訊息的旗標, 有需要傳出訊息的物件才設定
        {
            Notify_None = 0x00,
            Notify_Location = 0x01,
            Notify_Bounding = 0x02,
            Notify_Visibility = 0x04,
            Notify_CullMode = 0x08,
            Notify_RenderState = 0x10,
            Notify_All = 0xff,
        };
        using NotifyFlags = std::bitset<5>;

    public:
        Spatial(const SpatialId& id);
        Spatial(const Spatial&) = delete;
        Spatial(Spatial&&) = delete;
        virtual ~Spatial();
        Spatial& operator=(const Spatial&) = delete;
        Spatial& operator=(Spatial&&) = delete;

        //! ADR : 這裡傳回空的 assembler, 而不是已組裝的。如果要傳回已組裝的，會讓 disassembler 有概念侵入問題，在SpatialAssembler中實作靜態函式
        virtual std::shared_ptr<SpatialAssembler> assembler() const;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler);
        //! ADR : 這裡傳回空的 disassembler, 而不是已拆解的。如果要傳回已拆解的，必須把 DTO 參數傳入，這樣會讓 DTO 概念侵入，在SpatialDisassembler中實作靜態函式
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler);

        static std::shared_ptr<Spatial> querySpatial(const SpatialId& id);

        const SpatialId& id() const { return m_id; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }
        void persistenceLevel(PersistenceLevel level) { m_persistenceLevel = level; }

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        /** @name scene graph relation */
        //@{
        void linkParent(const std::optional<SpatialId>& parent);
        std::shared_ptr<Spatial> getParent() const;
        const std::optional<SpatialId>& getParentId() const { return m_parent; }
        unsigned int getGraphDepth() { return m_graphDepth; }
        void detachFromParent();  ///< parent should not call this function, it will be recursive forever!!
        //@}

        /** @name culling functions */
        //@{
        /** compute visible set, used by culler */
        virtual error cullVisibleSet(Culler* culler, bool noCull);
        virtual bool canVisited() = 0;
        /** on cull visible, used by culler, for compute visible set (recursive calling)  */
        virtual error onCullingVisible(Culler* culler, bool noCull) = 0;
        /** on complete not visible, only for top spatial, will not recursive */
        virtual void onCullingCompleteNotVisible(Culler*) {}
        //@}
        /** is renderable object?? */
        virtual bool isRenderable() { return false; }
        /** insert spatial object to renderer */
        virtual error insertToRenderer(const Engine::IRendererPtr&) { return error{}; }

        /** get world transform */
        MathLib::Matrix4& getWorldTransform() { return m_mxWorldTransform; }
        /** get local transform */
        MathLib::Matrix4& getLocalTransform() { return m_mxLocalTransform; }
        /** get model bound */
        const Engine::BoundingVolume& getModelBound() { return m_modelBound; }
        /** get world bound */
        const Engine::BoundingVolume& getWorldBound() { return m_worldBound; }
        /** get parent world transform */
        MathLib::Matrix4 getParentWorldTransform() const;

        /** @name set local position attribute, then modify local transform & update world transform/bound */
        //@{
        /** set local position */
        virtual error setLocalPosition(const MathLib::Vector3& pos);
        /** set local rotation by rotation matrix */
        virtual error setLocalRotation(const MathLib::Matrix3& rot);
        /** set local rotation by quaternion */
        virtual error setLocalRotation(const MathLib::Quaternion& qt);
        /** set local rotation by euler angle */
        virtual error setLocalEulerAngle(const MathLib::Vector3& angle);
        /** set local scale with uniform scaling */
        virtual error setLocalUniformScale(float scale);
        /** set local scale */
        virtual error setLocalScale(const MathLib::Vector3& scale);
        /** set local transform */
        virtual error setLocalTransform(const MathLib::Matrix4& mx);
        //@}

        /** @name get local position attribute */
        //@{
        /** get local position */
        MathLib::Vector3& getLocalPosition() { return m_vecLocalPosition; }
        /** get local rotation matrix */
        MathLib::Matrix3& getLocalRotation() { return m_mxLocalRotation; }
        /** get local rotation quaternion */
        MathLib::Quaternion& getLocalQuaternion() { return m_qtLocalQuaternion; }
        /** get local rotation euler angle */
        MathLib::Vector3& getLocalEulerAngle() { return m_vecLocalEulerAngle; }
        /** get local scale */
        MathLib::Vector3& getLocalScale() { return m_vecLocalScale; }
        //@}

        /** @name get world position attribute */
        //@{
        /** get world position */
        MathLib::Vector3& getWorldPosition() { return m_vecWorldPosition; }
        //@}

        /** change world position, and attach to new parent node */
        virtual void changeWorldPosition(const MathLib::Vector3& vecWorldPos, const std::optional<std::shared_ptr<Node>>& new_parent_option);

        /** @name scene traveler */
        //@{
        /** visit by traveler */
        virtual SceneTraveler::TravelResult visitBy(SceneTraveler* traveler);
        //@}

        /** culling mode, default is dynamic */
        void setCullingMode(CullingMode mode);
        /** get culling mode */
        CullingMode getCullingMode() { return m_cullingMode; }

        /** add spatial flag */
        void addSpatialFlag(SpatialFlags flag);
        /** remove spatial flag */
        void removeSpatialFlag(SpatialFlags flag);
        /** get spatial flag */
        SpatialFlags getSpatialFlag() { return m_spatialFlags; }
        /** test spatial flag */
        bool testSpatialFlag(SpatialFlags flag)
        {
            return (m_spatialFlags & flag).any();
        }

        /** notify spatial render state changed (after light changed,... etc.) */
        virtual void notifySpatialRenderStateChanged();

        /** @name inner public functions    */
        //@{
        virtual error _updateLocalTransform(const MathLib::Matrix4& mxLocal);
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld);
        virtual error _updateBoundData();

        /// call by parent
        virtual error _updateSpatialRenderState();
        /// notify parent to update me!!
        virtual error _propagateSpatialRenderState();
        //@}


        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void enumAnimatorListDeep(AnimatorList& resultList) override;

        /** add spatial notify flag */
        void addNotifyFlag(NotifyFlags flag)
        {
            m_notifyFlags |= flag;
        }
        /** remove spatial notify flag */
        void removeNotifyFlag(NotifyFlags flag)
        {
            m_notifyFlags &= (~flag);
        }
        /** get spatial notify flag */
        NotifyFlags getNotifyFlag() { return m_notifyFlags; };
        /** test spatial notify flag */
        bool testNotifyFlag(NotifyFlags flag)
        {
            return (m_notifyFlags & flag).any();
        }

        std::shared_ptr<Spatial> thisSpatial()
        {
            return shared_from_this();
        }
        std::shared_ptr<const Spatial> thisSpatial() const
        {
            return shared_from_this();
        }

    protected:
        SpatialId m_id;
        PersistenceLevel m_persistenceLevel;

        Engine::FactoryDesc m_factoryDesc;

        MathLib::Vector3 m_vecLocalPosition;
        MathLib::Vector3 m_vecLocalScale;
        MathLib::Matrix3 m_mxLocalRotation;
        MathLib::Quaternion m_qtLocalQuaternion;
        MathLib::Vector3 m_vecLocalEulerAngle;

        MathLib::Matrix4 m_mxLocalTransform;
        MathLib::Matrix4 m_mxWorldTransform;

        Engine::BoundingVolume m_modelBound;
        Engine::BoundingVolume m_worldBound;

        MathLib::Vector3 m_vecWorldPosition;

        std::optional<SpatialId> m_parent;
        unsigned int m_graphDepth;

        CullingMode m_cullingMode;

        SpatialRenderState m_spatialRenderState;

        SpatialFlags m_spatialFlags;  ///< default: "belong to parent"

        //todo : 先全開，之後再看效能決定要不要減少
        NotifyFlags m_notifyFlags;  ///< enqueue message when location/bound/visibility... has changed, default is all
    };
};


#endif // SPATIAL_H
