/*********************************************************************
 * \file   Sphere3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_SPHERE3_H
#define _MATH_SPHERE3_H

#include "Vector3.h"

namespace Enigma::MathLib
{
    /** Math Lib Sphere3
    @remarks
     The sphere is represented as |X-C| = R where C is the center and R is
     the radius.
    */
    class Sphere3
    {
    public:
        Sphere3();
        Sphere3(const Vector3& center, float radius);

        Vector3 Center() const { return m_center; }
        Vector3& Center() { return m_center; }
        float Radius() const { return m_radius; }
        float& Radius() { return m_radius; }

        bool operator== (const Sphere3& sphere) const; ///< 浮點數值比較
        bool operator!= (const Sphere3& sphere) const;  ///< 浮點數值比較

    private:
        Vector3 m_center;
        float m_radius;
    };
};

#endif // !_MATH_SPHERE3_H
