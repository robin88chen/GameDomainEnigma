/*********************************************************************
 * \file   Triangle3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_TRIANGLE3_H
#define _MATH_TRIANGLE3_H

#include "Vector3.h"

namespace Enigma::MathLib
{
    /** Math Lib Triangle3
    @remarks
    The triangle is represented as an array of three vertices, V0, V1,
    and V2.
    */
    class Triangle3
    {
    public:
        // construction
        Triangle3();
        Triangle3(const Vector3& vec0, const Vector3& vec1, const Vector3& vec2);
        Triangle3(const Vector3 vec[3]);

        Vector3 Vector(int i) const { return m_vec[i]; }
        Vector3& Vector(int i) { return m_vec[i]; }
        operator const Vector3* () const { return &m_vec[0]; }
        operator Vector3* () { return &m_vec[0]; }

        bool operator== (const Triangle3& tri) const;
        bool operator!= (const Triangle3& tri) const;

        // distance from the point Q to the triangle
        float DistanceTo(const Vector3& vec) const;

    protected:
        Vector3 m_vec[3];
    };
};

#endif // !_MATH_TRIANGLE3_H
