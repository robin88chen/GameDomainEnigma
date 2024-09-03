/*********************************************************************
 * \file   Frustum.h
 * \brief  Frustum class, entity, use shared_ptr, maintained by repository,
 *          (2023.9) should be a value object, not entity
 *          (2023.11) re-design, immutable value object
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "SceneGraphDefines.h"
#include "MathLib/Matrix4.h"
#include "MathLib/Radian.h"
#include "Frameworks/Rtti.h"
#include <memory>
#include <string>
#include <system_error>

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class FrustumAssembler;
    class FrustumDisassembler;

    /** Frustum class */
    class Frustum
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
        Frustum();
        Frustum(const Frustum&) = default;
        Frustum(Frustum&&) = default;
        virtual ~Frustum() = default;
        Frustum& operator=(const Frustum&) = default;
        Frustum& operator=(Frustum&&) = default;

        static Frustum fromPerspective(GraphicCoordSys hand, MathLib::Radian fov, float aspect, float n_plane, float f_plane);
        static Frustum fromOrtho(GraphicCoordSys hand, float near_w, float near_h, float n_plane, float f_plane);

        void assemble(const std::shared_ptr<FrustumAssembler>& assembler) const;
        void disassemble(const std::shared_ptr<FrustumDisassembler>& disassembler);

        GraphicCoordSys getCoordHandSys() const { return m_handCoord; }

        /** get fov */
        MathLib::Radian fov() const { return m_fov; };
        /** get near plane z */
        float nearPlaneZ() const { return m_nearPlaneZ; };
        /** get far plane z */
        float farPlaneZ() const { return m_farPlaneZ; };
        /** get aspect ratio */
        float aspectRatio() const { return m_aspectRatio; };
        /** get near width */
        float nearWidth() const { return m_nearWidth; };
        /** get near width */
        float nearHeight() const { return m_nearHeight; };

        /** get projection transform */
        const MathLib::Matrix4& projectionTransform() const { return m_mxProjTransform; };
        /** get projection type */
        ProjectionType projectionType() const { return m_projectionType; };

    protected:
        Frustum(GraphicCoordSys hand, ProjectionType proj);
        void constructProjectionTransform();
        /** 設定透視投影矩陣 */
        error setPerspectiveProjection(MathLib::Radian fov, float aspect, float n_plane, float f_plane);
        /**  設定平行投影矩陣 */
        error setOrthoProjection(float near_w, float near_h, float n_plane, float f_plane);

    protected:
        GraphicCoordSys m_handCoord;

        MathLib::Radian m_fov;  ///< fov角度(弳度), default = pi/4
        float m_nearPlaneZ;  ///< Near plane distance, default = 0.1
        float m_farPlaneZ;  ///< far plane distance, default = 100
        float m_aspectRatio;  ///< aspect ratio (width/height), default = 4/3
        float m_nearWidth;   ///< near width (use in ortho-projection), default = 40
        float m_nearHeight;  ///< near height (use in ortho-projection), default = 30

        MathLib::Matrix4 m_mxProjTransform;    ///< projection transform matrix

        ProjectionType m_projectionType;
    };
};


#endif // FRUSTUM_H
