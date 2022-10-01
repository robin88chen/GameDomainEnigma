/*********************************************************************
 * \file   Camera.h
 * \brief  camera class, entity, use shared_ptr, maintained by repository
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include "SceneGraphRepository.h"
#include "MathLib/Vector3.h"
#include <memory>
#include <string>
#include <system_error>

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class Camera : public std::enable_shared_from_this<Camera>
    {
    public:
        Camera(const std::string& name, GraphicCoordSys hand);
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        virtual ~Camera();
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;

        const std::string& GetName() const { return m_name; }

        /** set camera local axis -- camera pos to lookat, up axis */
        error SetCameraAxis(const MathLib::Vector3& eye_to_lookat, const MathLib::Vector3& up);
        /** set camera location */
        error SetCameraLocation(const MathLib::Vector3& pos);
        /** set camera frame */
        error SetCameraFrame(const MathLib::Vector3& eye, const MathLib::Vector3& eye_to_lookat, const MathLib::Vector3& up);

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
        virtual const MathLib::Matrix4& GetProjectionTransform() { return m_cullingFrustum->GetProjectionTransform(); };

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
