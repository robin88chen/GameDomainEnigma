/*********************************************************************
 * \file   Frustum.h
 * \brief  Frustum class, entity, use shared_ptr, maintained by repository
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "SceneGraphDefines.h"
#include "MathLib/Matrix4.h"
#include "Frameworks/Rtti.h"
#include "GameEngine/GenericDto.h"
#include <memory>
#include <string>
#include <system_error>

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    /** Frustum class */
    class Frustum : public std::enable_shared_from_this<Frustum>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        /** projection type */
        enum class ProjectionType
        {
            Ortho,
            Perspective
        };

    public:
        Frustum(const std::string& name, GraphicCoordSys hand, ProjectionType proj);
        Frustum(const Engine::GenericDto& dto);
        Frustum(const Frustum&) = delete;
        Frustum(Frustum&&) = delete;
        virtual ~Frustum();
        Frustum& operator=(const Frustum&) = delete;
        Frustum& operator=(Frustum&&) = delete;

        Engine::GenericDto SerializeDto();

        const std::string& GetName() const { return m_name; }
        GraphicCoordSys GetCoordHandSys() const { return m_handCoord; }
        /** 設定透視投影矩陣 */
        error SetPerspectiveProjection(float fov, float aspect, float n_plane, float f_plane);
        /**  設定平行投影矩陣 */
        error SetOrthoProjection(float near_w, float near_h, float n_plane, float f_plane);

        /** get fov */
        float GetFov() const { return m_fov; };
        /** get near plane z */
        float GetNearPlaneZ() const { return m_nearPlaneZ; };
        /** get far plane z */
        float GetFarPlaneZ() const { return m_farPlaneZ; };
        /** get aspect ratio */
        float GetAspectRatio() const { return m_aspectRatio; };
        /** get near width */
        float GetNearWidth() const { return m_nearWidth; };
        /** get near width */
        float GetNearHeight() const { return m_nearHeight; };

        /** change fov (for perspective only) */
        void ChangeFov(float fov);
        /** change near z */
        void ChangeNearZ(float nz);
        /** change far z */
        void ChangeFarZ(float fz);
        /** change aspect ratio (for perspective only) */
        void ChangeAspectRatio(float ratio);
        /** change near width (for orthographic only) */
        void ChangeNearWidth(float nw);
        /** change near height (for orthographic only) */
        void ChangeNearHeight(float nh);

        /** get projection transform */
        const MathLib::Matrix4& GetProjectionTransform() { return m_mxProjTransform; };
        /** get projection type */
        ProjectionType GetProjectionType() const { return m_projectionType; };

    protected:
        std::string m_name;
        GraphicCoordSys m_handCoord;

        float m_fov;  ///< fov角度(弳度), default = pi/4
        float m_nearPlaneZ;  ///< Near plane distance, default = 0.1
        float m_farPlaneZ;  ///< far plane distance, default = 100
        float m_aspectRatio;  ///< aspect ratio (width/height), default = 4/3
        float m_nearWidth;   ///< near width (use in ortho-projection), default = 40
        float m_nearHeight;  ///< near height (use in ortho-projection), default = 30

        MathLib::Matrix4 m_mxProjTransform;    ///< projection transform matrix

        ProjectionType m_projectionType;
    };

    using FrustumPtr = std::shared_ptr<Frustum>;
};


#endif // FRUSTUM_H
