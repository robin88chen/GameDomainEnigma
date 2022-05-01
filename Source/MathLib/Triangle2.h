/*********************************************************************
 * \file   Triangle2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_TRIANGLE2_H
#define _MATH_TRIANGLE2_H

#include "Vector2.h"

namespace Enigma::MathLib
{
    /** Math Lib Triangle2
    @remarks
     The triangle is represented as an array of three vertices, V0, V1,
     and V2.
    */
    class Triangle2
    {
    public:
        // construction
        Triangle2();
        Triangle2(const Vector2& vec0, const Vector2& vec1,
                  const Vector2& vec2);
        Triangle2(const Vector2 vec[3]);

        Vector2 Vector(int i) const { return m_vec[i]; }
        Vector2& Vector(int i) { return m_vec[i]; }
        operator const Vector2*() const { return &m_vec[0]; }
        operator Vector2*() { return &m_vec[0]; }

        bool operator==(const Triangle2& tri) const;
        bool operator!=(const Triangle2& tri) const;

        // distance from the point Q to the triangle
        float DistanceTo(const Vector2& vec) const;

    protected:
        Vector2 m_vec[3];
    };
};

#endif // !_MATH_TRIANGLE2_H
