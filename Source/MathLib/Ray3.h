/*********************************************************************
 * \file   Ray3.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_RAY3_H
#define _MATH_RAY3_H

#include "Vector3.h"

namespace Enigma::MathLib
{
    /** Math Lib Ray3
    @remarks
    The ray is represented as P+t*D, where P is the ray origin, D is a
    unit-length direction vector, and t >= 0.  The user must ensure that
    the direction vector satisfies this condition.
    */
    class Ray3
    {
    public:
        // construction
        Ray3() = default;
        Ray3(const Vector3& origin, const Vector3& direction);

        Vector3 origin() const { return m_origin; }
        Vector3& origin() { return m_origin; }
        Vector3 direction() const { return m_direction; }
        Vector3& direction() { return m_direction; }

        bool operator== (const Ray3& ray) const;    ///< 浮點數值比較
        bool operator!= (const Ray3& ray) const;    ///< 浮點數值比較

    private:
        Vector3 m_origin;
        Vector3 m_direction;
    };
};

#endif // !_MATH_RAY3_H
