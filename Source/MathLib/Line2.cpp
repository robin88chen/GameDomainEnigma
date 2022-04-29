#include "Line2.h"

using namespace Enigma::MathLib;

Line2::Line2(const Vector2& origin, const Vector2& direction) :
    m_origin(origin), m_direction(direction)
{
}

bool Line2::operator ==(const Line2& line) const
{
    return ((m_origin == line.m_origin) && (m_direction == line.m_direction));
}

bool Line2::operator !=(const Line2& line) const
{
    return ((m_origin != line.m_origin) || (m_direction != line.m_direction));
}
