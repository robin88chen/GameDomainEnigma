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
#include "MathLib/Ray3.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/SceneGraphDtos.h"

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

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        void CreatePrimaryCamera(const SceneGraph::CameraDto& dto);
        const std::shared_ptr<SceneGraph::Camera>& GetPrimaryCamera() const { return m_primaryCamera; }

        /** @name camera operations */
        //@{
        void CameraZoom(float dist);
        void CameraSphereRotate(float horz_angle, float vert_angle, const MathLib::Vector3& center = MathLib::Vector3::ZERO);
        void CameraMove(float dir_dist, float slide_dist);
        void CameraMoveXZ(float move_x, float move_z);
        void CameraShiftLookAt(const MathLib::Vector3& vecLookAt);
        //@}

        /** @name frustum operations */
        //@{
        void ChangeAspectRatio(float ratio);
        void ChangeFrustumFarPlane(float far_z);
        void ChangeFrustumNearPlane(float near_z);
        void ChangeFrustumFov(float fov);
        //@}
        MathLib::Ray3 GetPickerRay(float clip_space_x, float clip_space_y);

    protected:
        void OnTargetResized(const Frameworks::IEventPtr& e);
        void OnMouseRightBtnDrag(const Frameworks::IEventPtr& e);
        void OnMouseWheel(const Frameworks::IEventPtr& e);

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;
        std::shared_ptr<SceneGraph::Camera> m_primaryCamera;

        Frameworks::EventSubscriberPtr m_onTargetResized;
        Frameworks::EventSubscriberPtr m_onRightBtnDrag;
        Frameworks::EventSubscriberPtr m_onMouseWheel;
    };
}

#endif // _GAME_CAMERA_SERVICE_H
