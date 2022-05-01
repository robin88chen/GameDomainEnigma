/*********************************************************************
 * \file   Box2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_BOX2_H
#define _MATH_BOX2_H

#include "Vector2.h"
#include <array>

namespace Enigma::MathLib
{
    /** Math Lib Box2
    @remarks
    A box has center C, axis directions U[0] and U[1] (both unit-length
     vectors), and extents e[0] and e[1] (both nonnegative numbers).  A
     point X = C+y[0]*U[0]+y[1]*U[1] is inside or on the box whenever
     |y[i]| <= e[i] for all i.
   */
    class Box2
    {
    public:
        // construction
        Box2();
        Box2(const Vector2& center, const Vector2 axis[2], const float extent[2]);
        Box2(const Vector2& center, const Vector2& axis0, const Vector2& axis1, float extent0, float extent1);

        Vector2 Center() const { return m_center; }
        Vector2& Center() { return m_center; }
        Vector2 Axis(int index) const { return m_axis[index]; }
        Vector2& Axis(int index) { return m_axis[index]; }
        Vector2* Axis() { return m_axis; }
        const Vector2* Axis() const { return m_axis; }
        float Extent(int index) const { return m_extent[index]; }
        float& Extent(int index) { return m_extent[index]; }
        float* Extent() { return m_extent; }
        const float* Extent() const { return m_extent; }
        /**
        @remarks
        akVertex[4]={ (-x,-y),(+x,-y),(+x,+y),(-x,+y) }
        */
        std::array<Vector2, 4> ComputeVertices() const;

        bool operator== (const Box2& box) const; ///< 浮點數值比較
        bool operator!= (const Box2& box) const;    ///< 浮點數值比較

    private:
        Vector2 m_center;
        Vector2 m_axis[2];  ///< must be an orthonormal set of vectors
        float m_extent[2];         ///< must be nonnegative
    };
};

#endif // !_MATH_BOX2_H
