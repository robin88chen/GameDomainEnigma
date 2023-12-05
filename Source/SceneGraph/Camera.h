/*********************************************************************
 * \file   Camera.h
 * \brief  camera class, entity, use shared_ptr, maintained by repository
 *          zoom, move, rotate 等操作行為直接在物件內
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include "SceneGraphDefines.h"
#include "SpatialId.h"
#include "MathLib/Vector3.h"
#include "GameEngine/GenericDto.h"
#include "Frameworks/Rtti.h"
#include "MathLib/Matrix4.h"
#include "Frameworks/EventSubscriber.h"
#include "Frustum.h"
#include <memory>
#include <string>
#include <system_error>
#include <optional>

namespace Enigma::SceneGraph
{
    using error = std::error_code;
    using FrustumPtr = std::shared_ptr<Frustum>;

    class Camera : public std::enable_shared_from_this<Camera>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        Camera(const SpatialId& id, GraphicCoordSys hand);
        Camera(const SpatialId& id, const Engine::GenericDto& dto);
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        virtual ~Camera();
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        Engine::GenericDto serializeDto();

        const SpatialId& id() const { return m_id; }
        GraphicCoordSys getCoordHandSys() const { return m_handSys; }

        /** change camera frame, eye_to_lookat & up must both set together or both not set   */
        error changeCameraFrame(const std::optional<MathLib::Vector3>& eye,
            const std::optional<MathLib::Vector3>& eye_to_lookat, const std::optional<MathLib::Vector3>& up);

        /** @name camera operations */
        //@{
        error zoom(float dist);
        error sphereRotate(float horz_angle, float vert_angle, const MathLib::Vector3& center = MathLib::Vector3::ZERO);
        error move(float dir_dist, float slide_dist);
        error moveXZ(float move_x, float move_z);
        error shiftLookAt(const MathLib::Vector3& vecLookAt);
        //@}

        MathLib::Vector3 location() const { return m_vecLocation; };
        MathLib::Vector3 eyeToLookatVector() const { return m_vecEyeToLookAt; };
        MathLib::Vector3 upVector() const { return m_vecUp; };
        MathLib::Vector3 rightVector() const { return m_vecRight; };

        void changeAspectRatio(float ratio);
        void changeFrustumFarPlane(float far_z);
        void changeFrustumNearPlane(float near_z);
        void changeFrustumFov(float fov);

        /** set frustum */
        error cullingFrustum(const Frustum& frustum);
        /** get frustum */
        const Frustum& cullingFrustum() const { return m_cullingFrustum; };

        /** get view transform */
        virtual const MathLib::Matrix4& viewTransform() { return m_mxViewTransform; };
        /** get projection transform */
        virtual const MathLib::Matrix4& projectionTransform();

        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void EnumAnimatorListDeep(AnimatorList& pResultList) override;

    protected:
        virtual void _updateViewTransform();

    protected:
        SpatialId m_id;
        Engine::FactoryDesc m_factoryDesc;
        GraphicCoordSys m_handSys;

        MathLib::Matrix4 m_mxViewTransform;    ///< view transform matrix

        Frustum m_cullingFrustum; ///< frustum of this camera, create/destroy with camera, can be changed by application
        MathLib::Vector3 m_vecLocation;
        MathLib::Vector3 m_vecEyeToLookAt;
        MathLib::Vector3 m_vecUp;
        MathLib::Vector3 m_vecRight;
        MathLib::Vector3 m_vecCameraForward;
    };
}

#endif // CAMERA_H
