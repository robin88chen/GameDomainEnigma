/*********************************************************************
 * \file   Line2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_LINE2_H
#define _MATH_LINE2_H

#include "Vector2.h"

namespace Enigma::MathLib
{
    /** Math Lib Line2
    @remarks
     The line is represented as P+t*D where P is the line origin and D is
     a unit-length direction vector.  The user must ensure that the
     direction vector satisfies this condition.
     */
    class Line2
    {
    public:
        Line2() = default;
        Line2(const Vector2& origin, const Vector2& direction);

        Vector2 origin() const { return m_origin; }
        Vector2& origin() { return m_origin; }
        Vector2 direction() const { return m_direction; }
        Vector2& direction() { return m_direction; }

        bool operator== (const Line2& line) const; ///< 浮點數值比較
        bool operator!= (const Line2& line) const;  ///< 浮點數值比較

    private:
        Vector2 m_origin;
        Vector2 m_direction;
    };

};

#endif // !_MATH_LINE2_H
