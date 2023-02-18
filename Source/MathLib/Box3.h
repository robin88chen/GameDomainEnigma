/*********************************************************************
 * \file   Box3.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_BOX3_H
#define _MATH_BOX3_H

#include "Vector3.h"
#include <array>

namespace Enigma::MathLib
{
    /** Math Lib Box3
    @remarks
     A box has center C, axis directions U[0], U[1], and U[2] (all
     unit-length vectors), and extents e[0], e[1], and e[2] (all nonnegative
     numbers).  A point X = C+y[0]*U[0]+y[1]*U[1]+y[2]*U[2] is inside or
     on the box whenever |y[i]| <= e[i] for all i.
    */
    class Box3
    {
    public:
        Box3();
        Box3(const Vector3& center, const Vector3 axis[3], const float extent[3]);
        Box3(const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2,
            float extent0, float extent1, float extent2);

        Vector3 Center() const { return m_center; }
        Vector3& Center() { return m_center; }
        Vector3 Axis(int index) const { return m_axis[index]; }
        Vector3& Axis(int index) { return m_axis[index]; }
        Vector3* Axis() { return m_axis; }
        const Vector3* Axis() const { return m_axis; }
        float Extent(int index) const { return m_extent[index]; }
        float& Extent(int index) { return m_extent[index]; }
        float* Extent() { return m_extent; }
        const float* Extent() const { return m_extent; }

        /**
        @remarks
        akVertex[8]={ (-x,-y,-z),(+x,-y,-z),(+x,+y,-z),(-x,+y,-z),
        (-x,-y,+z),(+x,-y,+z),(+x,+y,+z),(-x,+y,+z) }
        */
        std::array<Vector3, 8> ComputeVertices() const;

        /** swap to major axis (把軸向量取最大值對齊x,y,z) */
        Box3 SwapToMajorAxis() const;

        bool operator== (const Box3& box) const; ///< 浮點數值比較
        bool operator!= (const Box3& box) const;    ///< 浮點數值比較

        bool IsEmpty() const;

        static const Box3 UNIT_BOX;

    private:
        /** swap to major axis (把軸向量取最大值對齊x,y,z) */
        void ImplicitSwapToMajorAxis();
    private:
        Vector3 m_center;
        Vector3 m_axis[3];  ///< must be an orthonormal set of vectors
        float m_extent[3];         ///< must be nonnegative
    };
};

#endif // !_MATH_BOX3_H
