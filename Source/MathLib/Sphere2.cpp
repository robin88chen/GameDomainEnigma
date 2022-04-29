#include "Sphere2.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

Sphere2::Sphere2()
{
    m_center = Vector2::ZERO;
    m_radius = (float)0.0;
}

Sphere2::Sphere2(const Vector2& center, float radius) :
    m_center(center), m_radius(radius)
{
}

bool Sphere2::operator == (const Sphere2& sphere) const
{
    return ((m_center == sphere.m_center) && (Math::IsEqual(m_radius, sphere.m_radius)));
}

bool Sphere2::operator != (const Sphere2& sphere) const
{
    return ((m_center != sphere.m_center) || (!Math::IsEqual(m_radius, sphere.m_radius)));
}
