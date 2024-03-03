/*********************************************************************
 * \file   SceneGraphRepository.h
 * \brief  scene graph objects repository, maintain objects lifecycle
 *      and some global settings
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_REPOSITORY_H
#define SCENE_GRAPH_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "GameEngine/DtoDeserializer.h"
#include "SceneGraphDefines.h"
#include "Frustum.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "GameEngine/FactoryDesc.h"
#include "SpatialId.h"
#include "SceneGraphPersistenceLevel.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace Enigma::SceneGraph
{
    class SceneGraphStoreMapper;
    class SceneGraphFactory;
    class Spatial;
    class Camera;
    class Frustum;
    class Node;
    class Pawn;
    class Portal;
    class LightInfo;
    class Light;
    class LazyNode;
    class PortalZoneNode;
    class VisibilityManagedNode;
    class CameraDto;
    class NodeDto;
    class LazyNodeDto;
    class VisibilityManagedNodeDto;
    class LightDto;
    class PawnDto;
    class PortalDto;
    class PortalZoneNodeDto;
    class PortalManagementNodeDto;
    class SceneGraphBuilder;

    class SceneGraphRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneGraphRepository(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper);
        SceneGraphRepository(const SceneGraphRepository&) = delete;
        SceneGraphRepository(SceneGraphRepository&&) = delete;
        virtual ~SceneGraphRepository() override;
        SceneGraphRepository& operator=(const SceneGraphRepository&) = delete;
        SceneGraphRepository& operator=(SceneGraphRepository&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        void setCoordinateSystem(GraphicCoordSys hand);
        GraphicCoordSys getCoordinateSystem();

        SceneGraphFactory* factory() { return m_factory; }

        //std::shared_ptr<Camera> createCamera(const SpatialId& id);
        //std::shared_ptr<Camera> createCamera(const Engine::GenericDto& dto);

        std::shared_ptr<Node> createNode(const SpatialId& id);
        std::shared_ptr<Node> createNode(const std::string& name, const Engine::FactoryDesc& factory_desc);
        std::shared_ptr<Node> createNode(const Engine::GenericDto& dto);

        //std::shared_ptr<Pawn> CreatePawn(const std::string& name);

        //std::shared_ptr<Light> createLight(const SpatialId& id, const LightInfo& info);
        //bool hasLight(const SpatialId& id);
        //std::shared_ptr<Light> queryLight(const SpatialId& id);

        std::shared_ptr<Portal> createPortal(const std::string& name);
        bool hasPortal(const std::string& name);
        std::shared_ptr<Portal> queryPortal(const std::string& name);

        std::shared_ptr<Spatial> querySpatial(const std::string& name);
        std::shared_ptr<Spatial> AddNewSpatial(Spatial* spatial);

        bool hasNode(const std::string& name);
        std::shared_ptr<Node> queryNode(const std::string& name);

        /** query entities */
        bool hasCamera(const SpatialId& id);
        std::shared_ptr<Camera> queryCamera(const SpatialId& id);
        bool hasSpatial(const SpatialId& id);
        std::shared_ptr<Spatial> querySpatial(const SpatialId& id);

        /** put entities */
        void putCamera(const std::shared_ptr<Camera>& camera);
        void putSpatial(const std::shared_ptr<Spatial>& spatial, PersistenceLevel persistence_level);

        /** remove entities */
        void removeCamera(const SpatialId& id);
        void removeSpatial(const SpatialId& id);

        /** factory methods */
        std::shared_ptr<PortalZoneNode> createPortalZoneNode(const PortalZoneNodeDto& portal_zone_node_dto);
        std::shared_ptr<VisibilityManagedNode> createVisibilityManagedNode(const VisibilityManagedNodeDto& visibility_managed_node_dto);

    private:
        void registerHandlers();
        void unregisterHandlers();

        void queryCamera(const Frameworks::IQueryPtr& q);
        void requestCameraCreation(const Frameworks::IQueryPtr& r);
        void requestCameraConstitution(const Frameworks::IQueryPtr& r);
        void queryNode(const Frameworks::IQueryPtr& q);
        void querySpatial(const Frameworks::IQueryPtr& q);
        void requestSpatialCreation(const Frameworks::IQueryPtr& r);
        void requestSpatialConstitution(const Frameworks::IQueryPtr& r);
        void requestLightCreation(const Frameworks::IQueryPtr& r);

        void putCamera(const Frameworks::ICommandPtr& c);
        void removeCamera(const Frameworks::ICommandPtr& c);
        void putSpatial(const Frameworks::ICommandPtr& c);
        void removeSpatial(const Frameworks::ICommandPtr& c);

        void attachNodeChild(const Frameworks::ICommandPtr& c);

        //void createCamera(const Frameworks::ICommandPtr& c);
        //void createNode(const Frameworks::ICommandPtr& c);

    private:
        SceneGraphFactory* m_factory;
        std::shared_ptr<SceneGraphStoreMapper> m_storeMapper;
        GraphicCoordSys m_handSystem;

        std::unordered_map<SpatialId, std::shared_ptr<Camera>, SpatialId::hash> m_cameras;
        std::recursive_mutex m_cameraMapLock;

        std::unordered_map<std::string, std::weak_ptr<Node>> m_nodes;
        std::recursive_mutex m_nodeMapLock;
        std::unordered_map<SpatialId, std::shared_ptr<Spatial>, SpatialId::hash> m_spatials;
        std::recursive_mutex m_spatialMapLock;

        std::unordered_map<SpatialId, std::weak_ptr<Light>, SpatialId::hash> m_lights;
        std::recursive_mutex m_lightMapLock;

        std::unordered_map<std::string, std::weak_ptr<Portal>> m_portals;
        std::recursive_mutex m_portalMapLock;

        SceneGraphBuilder* m_builder;

        Frameworks::QuerySubscriberPtr m_queryCamera;
        Frameworks::QuerySubscriberPtr m_requestCameraCreation;
        Frameworks::QuerySubscriberPtr m_requestCameraConstitution;
        Frameworks::QuerySubscriberPtr m_queryNode;
        Frameworks::QuerySubscriberPtr m_querySpatial;
        Frameworks::QuerySubscriberPtr m_requestSpatialCreation;
        Frameworks::QuerySubscriberPtr m_requestSpatialConstitution;
        Frameworks::QuerySubscriberPtr m_requestLightCreation;

        Frameworks::CommandSubscriberPtr m_putCamera;
        Frameworks::CommandSubscriberPtr m_removeCamera;
        Frameworks::CommandSubscriberPtr m_putSpatial;
        Frameworks::CommandSubscriberPtr m_removeSpatial;

        Frameworks::CommandSubscriberPtr m_attachNodeChild;

        //Frameworks::CommandSubscriberPtr m_createCamera;
        //Frameworks::CommandSubscriberPtr m_createNode;
    };
}

#endif // SCENE_GRAPH_REPOSITORY_H
