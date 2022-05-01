/********************************************************************
 * \file   MathAlgorithm.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_ALGORITHM_H
#define _MATH_ALGORITHM_H

#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"

namespace Enigma::MathLib
{
    class MathAlgorithm
    {
    public:
        struct TangentBinormalPair
        {
            Vector3 m_tangent;
            Vector3 m_binormal;
        };
    public:
        /** Make Perspective Projection (Fov, Left Hand Coord.)
        @param fovy y方向fov
        @param aspect =width/height
        @param nearPlane near plane z value
        @param farPlane far plane z value
        @remark
        <pre>
        xScale     0          0               0
        0        yScale       0               0
        0          0       zf/(zf-zn)      -zn*zf/(zf-zn)
        0          0          1               0
        where:
        yScale = cot(fovY/2)
        xScale = yScale / aspect ratio
        </pre>
        */
        static Matrix4 MakePerspectiveProjectionFovLH(float fovy, float aspect, float nearPlane, float farPlane);

        /** Make Perspective Projection (Fov, Right Hand Coord.)
        @param fovy y方向fov
        @param aspect =width/height
        @param nearPlane near plane z value
        @param farPlane far plane z value
        @remark
        <pre>
        xScale     0          0               0
        0        yScale       0               0
        0          0       zf/(zn-zf)      zn*zf/(zn-zf)
        0          0          -1              0
        where:
        yScale = cot(fovY/2)
        xScale = yScale / aspect ratio
        </pre>
        */
        static Matrix4 MakePerspectiveProjectionFovRH(float fovy, float aspect, float nearPlane, float farPlane);

        /** Make Perspective Projection (Near plane width & height, Left Hand Coord.)
        @param w near plane width
        @param h near plane height
        @param nearPlane near plane z value
        @param farPlane far plane z value
        @remark
        <pre>
        2*zn/w  0       0              0
        0       2*zn/h  0              0
        0       0       zf/(zf-zn)  zn*zf/(zn-zf)
        0       0       1              0
        </pre>
        */
        static Matrix4 MakePerspectiveProjectionLH(float w, float h, float nearPlane, float farPlane);

        /** Make Perspective Projection (Near plane width & height, Right Hand Coord.)
        @param w near plane width
        @param h near plane height
        @param nearPlane near plane z value
        @param farPlane far plane z value
        @remark
        <pre>
        2*zn/w  0       0              0
        0       2*zn/h  0              0
        0       0       zf/(zn-zf)  zn*zf/(zn-zf)
        0       0       -1             0
        </pre>
        */
        static Matrix4 MakePerspectiveProjectionRH(float w, float h, float nearPlane, float farPlane);

        /** Make LookAt Transform (Left Hand Coord.)
        @param Eye Eye Position
        @param At Look-at Position
        @param Up Camera Up Vector
        @remark
        <pre>
        zaxis = normal(At - Eye)
        xaxis = normal(cross(Up, zaxis))
        yaxis = cross(zaxis, xaxis)
        xaxis.x         xaxis.y           xaxis.z          -dot(xaxis,eye)
        yaxis.x         yaxis.y           yaxis.z          -dot(yaxis,eye)
        zaxis.x         zaxis.y           zaxis.z          -dot(zaxis,eye)
        0                             0                               0                               1
        </pre>
        */
        static Matrix4 MakeLookAtTransformLH(const Vector3& eye, const Vector3& at, const Vector3& up);

        /** Make Ortho- Projection (Left Hand Coord.)
        @param w near plane width
        @param h near plane height
        @param zn near plane z value
        @param zf far plane z value
        @remark
        <pre>
        2/w  0    0           0
        0    2/h  0           0
        0    0    1/(zf-zn)   zn/(zn-zf)
        0    0    0           1
        </pre>
        */
        static Matrix4 MakeOrthoProjectionLH(float w, float h, float zn, float zf);

        /** Make Ortho- Projection (Right Hand Coord.)
        @param w near plane width
        @param h near plane height
        @param zn near plane z value
        @param zf far plane z value
        @remark
        <pre>
        2/w  0    0           0
        0    2/h  0           0
        0    0    1/(zn-zf)   zn/(zn-zf)
        0    0    0           1
        </pre>
        */
        static Matrix4 MakeOrthoProjectionRH(float w, float h, float zn, float zf);

        /** calculate tangent space */
        static TangentBinormalPair CalculateTangentVector(const Vector3 vec[3], const Vector2 uv[3]);
    };
}
#endif // !_MATH_ALGORITHM_H
