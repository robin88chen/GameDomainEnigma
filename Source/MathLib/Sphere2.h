/*********************************************************************
 * \file   Sphere2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_SPHERE2_H
#define _MATH_SPHERE2_H

#include "Vector2.h"

namespace Enigma::MathLib
{
    /** Math Lib Sphere2
    @remarks
     The sphere (circle) is represented as |X-C| = R where C is the center and R is
     the radius.
    */
    class Sphere2
    {
    public:
        Sphere2();
        Sphere2(const Vector2& center, float radius);

        Vector2 Center() const { return m_center; }
        Vector2& Center() { return m_center; }
        float Radius() const { return m_radius; }
        float& Radius() { return m_radius; }

        bool operator== (const Sphere2& sphere) const; ///< 浮點數值比較
        bool operator!= (const Sphere2& sphere) const;  ///< 浮點數值比較

    private:
        Vector2 m_center;
        float m_radius;
    };
};

#endif // !_MATH_SPHERE2_H
