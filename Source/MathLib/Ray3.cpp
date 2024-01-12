#include "Ray3.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

Ray3::Ray3(const Vector3& origin, const Vector3& direction) :
    m_origin(origin), m_direction(direction)
{
}

bool Ray3::operator ==(const Ray3& ray) const
{
    return ((m_origin == ray.m_origin) && (m_direction == ray.m_direction));
}

bool Ray3::operator !=(const Ray3& ray) const
{
    return ((m_origin != ray.m_origin) || (m_direction != ray.m_direction));
}
