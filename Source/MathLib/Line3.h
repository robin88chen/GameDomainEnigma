/*********************************************************************
 * \file   Line3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_LINE3_H
#define _MATH_LINE3_H

#include "Vector3.h"

namespace Enigma::MathLib
{
    /** Math Lib Line3
    @remarks
    The line is represented as P+t*D where P is the line origin and D is
    a unit-length direction vector.  The user must ensure that the
    direction vector satisfies this condition.
    */
    class Line3
    {
    public:
        Line3() = default;
        Line3(const Vector3& origin, const Vector3& direction);

        Vector3 origin() const { return m_origin; }
        Vector3& origin() { return m_origin; }
        Vector3 direction() const { return m_direction; }
        Vector3& direction() { return m_direction; }

        bool operator== (const Line3& line) const; ///< 浮點數值比較
        bool operator!= (const Line3& line) const;  ///< 浮點數值比較

    private:
        Vector3 m_origin;
        Vector3 m_direction;
    };

};

#endif // !_MATH_LINE3_H
