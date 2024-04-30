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
#include "SceneGraphDefines.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
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
    class Node;
    class Portal;
    class LazyNode;
    class PortalZoneNode;
    class PortalZoneNodeDto;

    class SceneGraphRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneGraphRepository(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper);
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

        /** query entities */
        bool hasCamera(const SpatialId& id);
        std::shared_ptr<Camera> queryCamera(const SpatialId& id);
        bool hasSpatial(const SpatialId& id);
        std::shared_ptr<Spatial> querySpatial(const SpatialId& id);
        bool hasLaziedContent(const SpatialId& id);
        void hydrateLazyNode(const SpatialId& id);

        /** put entities */
        void putCamera(const std::shared_ptr<Camera>& camera);
        void putSpatial(const std::shared_ptr<Spatial>& spatial, PersistenceLevel persistence_level);
        void putLaziedContent(const std::shared_ptr<LazyNode>& lazy_node);

        /** remove entities */
        void removeCamera(const SpatialId& id);
        void removeSpatial(const SpatialId& id);
        void removeLaziedContent(const SpatialId& id);

    private:
        void registerHandlers();
        void unregisterHandlers();

        void queryCamera(const Frameworks::IQueryPtr& q);
        void requestCameraCreation(const Frameworks::IQueryPtr& r);
        void requestCameraConstitution(const Frameworks::IQueryPtr& r);
        void querySpatial(const Frameworks::IQueryPtr& q);
        void hasSpatial(const Frameworks::IQueryPtr& q);
        void requestSpatialCreation(const Frameworks::IQueryPtr& r);
        void requestSpatialConstitution(const Frameworks::IQueryPtr& r);
        void requestLightCreation(const Frameworks::IQueryPtr& r);

        void putCamera(const Frameworks::ICommandPtr& c);
        void removeCamera(const Frameworks::ICommandPtr& c);
        void putSpatial(const Frameworks::ICommandPtr& c);
        void removeSpatial(const Frameworks::ICommandPtr& c);
        void putLaziedContent(const Frameworks::ICommandPtr& c);
        void removeLaziedContent(const Frameworks::ICommandPtr& c);

    private:
        SceneGraphFactory* m_factory;
        std::shared_ptr<SceneGraphStoreMapper> m_storeMapper;
        GraphicCoordSys m_handSystem;

        std::unordered_map<SpatialId, std::shared_ptr<Camera>, SpatialId::hash> m_cameras;
        std::recursive_mutex m_cameraMapLock;

        std::unordered_map<SpatialId, std::shared_ptr<Spatial>, SpatialId::hash> m_spatials;
        std::recursive_mutex m_spatialMapLock;

        Frameworks::QuerySubscriberPtr m_queryCamera;
        Frameworks::QuerySubscriberPtr m_requestCameraCreation;
        Frameworks::QuerySubscriberPtr m_requestCameraConstitution;
        Frameworks::QuerySubscriberPtr m_querySpatial;
        Frameworks::QuerySubscriberPtr m_hasSpatial;
        Frameworks::QuerySubscriberPtr m_requestSpatialCreation;
        Frameworks::QuerySubscriberPtr m_requestSpatialConstitution;
        Frameworks::QuerySubscriberPtr m_requestLightCreation;

        Frameworks::CommandSubscriberPtr m_putCamera;
        Frameworks::CommandSubscriberPtr m_removeCamera;
        Frameworks::CommandSubscriberPtr m_putSpatial;
        Frameworks::CommandSubscriberPtr m_removeSpatial;
        Frameworks::CommandSubscriberPtr m_putLaziedContent;
        Frameworks::CommandSubscriberPtr m_removeLaziedContent;

    };
}

#endif // SCENE_GRAPH_REPOSITORY_H
