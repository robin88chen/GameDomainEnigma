/********************************************************************
 * \file   Rect.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_RECT_H
#define _MATH_RECT_H

namespace Enigma::MathLib
{
    /** Math Lib Rect */
    class Rect
    {
    public:
        Rect() = default;
        Rect(int left, int top, int right, int bottom);

        /** @name  attribute */
        //@{
        int Left() const;
        int& Left();
        int Top() const;
        int& Top();
        int Right() const;
        int& Right();
        int Bottom() const;
        int& Bottom();
        int Width() const;
        int Height() const;
        //@}

    private:
        int m_left;
        int m_top;
        int m_right;
        int m_bottom;
    };
};

#endif
