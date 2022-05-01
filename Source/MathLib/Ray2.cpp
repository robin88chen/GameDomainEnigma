#include "Ray2.h"

using namespace Enigma::MathLib;

Ray2::Ray2(const Vector2& origin, const Vector2& direction) :
    m_origin(origin), m_direction(direction)
{
}

bool Ray2::operator ==(const Ray2& ray) const
{
    return ((m_origin == ray.m_origin) && (m_direction == ray.m_direction));
}

bool Ray2::operator !=(const Ray2& ray) const
{
    return ((m_origin != ray.m_origin) || (m_direction != ray.m_direction));
}
