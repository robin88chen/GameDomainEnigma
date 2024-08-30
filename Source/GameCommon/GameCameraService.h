/*********************************************************************
 * \file   GameCameraService.h
 * \brief game camera service
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GAME_CAMERA_SERVICE_H
#define _GAME_CAMERA_SERVICE_H

#include "Frameworks/SystemService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/SpatialId.h"
#include "MathLib/Ray3.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "SceneGraph/CameraDtos.h"

namespace Enigma::GameCommon
{
    class GameCameraService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GameCameraService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository);
        GameCameraService(const GameCameraService&) = delete;
        GameCameraService(GameCameraService&&) = delete;
        virtual ~GameCameraService() override;
        GameCameraService& operator=(const GameCameraService&) = delete;
        GameCameraService& operator=(GameCameraService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        void constitutePrimaryCamera(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto);
        const std::shared_ptr<SceneGraph::Camera>& primaryCamera() const { return m_primaryCamera; }

        /** @name camera operations */
        //@{
        void zoom(float dist);
        void sphereRotate(float horz_angle, float vert_angle, const MathLib::Vector3& center = MathLib::Vector3::ZERO);
        void move(float dir_dist, float slide_dist);
        void moveXZ(float move_x, float move_z);
        void shiftLookAt(const MathLib::Vector3& vecLookAt);
        //@}

        /** @name frustum operations */
        //@{
        void changeAspectRatio(float ratio);
        void changeFrustumFarPlane(float far_z);
        void changeFrustumNearPlane(float near_z);
        void changeFrustumFov(MathLib::Radian fov);
        //@}
        MathLib::Ray3 getPickerRay(float clip_space_x, float clip_space_y);

    protected:
        void onTargetResized(const Frameworks::IEventPtr& e);
        //todo: move to UI layer
        void onMouseRightBtnDrag(const Frameworks::IEventPtr& e);
        void onMouseWheel(const Frameworks::IEventPtr& e);
        void onGestureScroll(const Frameworks::IEventPtr& e);
        void onGestureScale(const Frameworks::IEventPtr& e);

        void zoom(const Frameworks::ICommandPtr& c);
        void sphereRotate(const Frameworks::ICommandPtr& c);
        void move(const Frameworks::ICommandPtr& c);
        void moveXZ(const Frameworks::ICommandPtr& c);

        void queryPrimaryCamera(const Frameworks::IQueryPtr& q);

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;
        std::shared_ptr<SceneGraph::Camera> m_primaryCamera;

        Frameworks::EventSubscriberPtr m_onTargetResized;
        Frameworks::EventSubscriberPtr m_onRightBtnDrag;
        Frameworks::EventSubscriberPtr m_onMouseWheel;
        Frameworks::EventSubscriberPtr m_onGestureScroll;
        Frameworks::EventSubscriberPtr m_onGestureScale;

        Frameworks::CommandSubscriberPtr m_zoomCamera;
        Frameworks::CommandSubscriberPtr m_sphereRotateCamera;
        Frameworks::CommandSubscriberPtr m_moveCamera;
        Frameworks::CommandSubscriberPtr m_moveCameraXZ;

        Frameworks::QuerySubscriberPtr m_queryPrimaryCamera;
    };
}

#endif // _GAME_CAMERA_SERVICE_H
