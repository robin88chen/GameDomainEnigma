#include "Line3.h"

using namespace Enigma::MathLib;

Line3::Line3(const Vector3& origin, const Vector3& direction) :
    m_origin(origin), m_direction(direction)
{
}

bool Line3::operator ==(const Line3& line) const
{
    return ((m_origin == line.m_origin) && (m_direction == line.m_direction));
}

bool Line3::operator !=(const Line3& line) const
{
    return ((m_origin != line.m_origin) || (m_direction != line.m_direction));
}
