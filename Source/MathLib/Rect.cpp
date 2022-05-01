#include "Rect.h"
#include <cassert>

using namespace Enigma::MathLib;

Rect::Rect(int left, int top, int right, int bottom)
{
    assert(right >= left);
    assert(bottom >= top);
    m_left = left;
    m_top = top;
    m_right = right;
    m_bottom = bottom;
}

int Rect::Left() const
{
    return m_left;
}

int Rect::Top() const
{
    return m_top;
}

int Rect::Right() const
{
    return m_right;
}

int Rect::Bottom() const
{
    return m_bottom;
}

int Rect::Width() const
{
    return m_right - m_left;
}

int Rect::Height() const
{
    return m_bottom - m_top;
}
