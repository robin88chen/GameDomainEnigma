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
#include "MathLib/Vector3.h"
#include "GameEngine/GenericDto.h"
#include "Frameworks/Rtti.h"
#include "MathLib/Matrix4.h"
#include <memory>
#include <string>
#include <system_error>
#include <optional>

namespace Enigma::SceneGraph
{
    class CameraDto;
    class Frustum;
    using error = std::error_code;
    using FrustumPtr = std::shared_ptr<Frustum>;

    class Camera : public std::enable_shared_from_this<Camera>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        Camera(const std::string& name, GraphicCoordSys hand);
        Camera(const CameraDto& dto);
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        virtual ~Camera();
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;

        Engine::GenericDto SerializeDto();

        const std::string& GetName() const { return m_name; }
        GraphicCoordSys GetCoordHandSys() const { return m_handSys; }

        /** change camera frame, eye_to_lookat & up must both set together or both not set   */
        error ChangeCameraFrame(const std::optional<MathLib::Vector3>& eye,
            const std::optional<MathLib::Vector3>& eye_to_lookat, const std::optional<MathLib::Vector3>& up);

        /** @name camera operations */
        //@{
        error Zoom(float dist);
        error SphereRotate(float horz_angle, float vert_angle, const MathLib::Vector3& center = MathLib::Vector3::ZERO);
        error Move(float dir_dist, float slide_dist);
        error MoveXZ(float move_x, float move_z);
        error ShiftLookAt(const MathLib::Vector3& vecLookAt);
        //@}

        MathLib::Vector3 GetLocation() const { return m_vecLocation; };
        MathLib::Vector3 GetEyeToLookatVector() const { return m_vecEyeToLookAt; };
        MathLib::Vector3 GetUpVector() const { return m_vecUp; };
        MathLib::Vector3 GetRightVector() const { return m_vecRight; };

        /** set frustum */
        error SetCullingFrustum(const FrustumPtr& frustum);
        /** get frustum */
        FrustumPtr GetCullingFrustum() { return m_cullingFrustum; };

        /** get view transform */
        virtual const MathLib::Matrix4& GetViewTransform() { return m_mxViewTransform; };
        /** get projection transform */
        virtual const MathLib::Matrix4& GetProjectionTransform();

        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void EnumAnimatorListDeep(AnimatorList& pResultList) override;

    protected:
        virtual void _UpdateViewTransform();

    protected:
        std::string m_name;
        GraphicCoordSys m_handSys;

        MathLib::Matrix4 m_mxViewTransform;    ///< view transform matrix

        FrustumPtr m_cullingFrustum; ///< frustum of this camera, create/destroy with camera, can be changed by application

        MathLib::Vector3 m_vecLocation;
        MathLib::Vector3 m_vecEyeToLookAt;
        MathLib::Vector3 m_vecUp;
        MathLib::Vector3 m_vecRight;
        MathLib::Vector3 m_vecCameraForward;
    };
}

#endif // CAMERA_H
