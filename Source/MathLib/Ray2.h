/*********************************************************************
 * \file   Ray2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_RAY2_H
#define _MATH_RAY2_H

#include "Vector2.h"

namespace Enigma::MathLib
{
    /** Math Lib Ray2
    @remarks
     The ray is represented as P+t*D, where P is the ray origin, D is a
     unit-length direction vector, and t >= 0.  The user must ensure that
     the direction vector satisfies this condition.
    */
    class Ray2
    {
    public:
        // construction
        Ray2() = default;
        Ray2(const Vector2& origin, const Vector2& direction);

        Vector2 origin() const { return m_origin; }
        Vector2& origin() { return m_origin; }
        Vector2 direction() const { return m_direction; }
        Vector2& direction() { return m_direction; }

        bool operator== (const Ray2& ray) const;    ///< 浮點數值比較
        bool operator!= (const Ray2& ray) const;    ///< 浮點數值比較

    private:
        Vector2 m_origin;
        Vector2 m_direction;
    };
};

#endif // !_MATH_RAY2_H
