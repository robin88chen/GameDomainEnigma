#include "Sphere3.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

const Sphere3 Sphere3::UNIT_SPHERE(Vector3(0.0f, 0.0f, 0.0f), 1.0f);

Sphere3::Sphere3()
{
    m_center = Vector3::ZERO;
    m_radius = (float)0.0;
}

Sphere3::Sphere3(const Vector3& center, float radius) :
    m_center(center), m_radius(radius)
{
}

bool Sphere3::operator == (const Sphere3& sphere) const
{
    return ((m_center == sphere.m_center) && (Math::IsEqual(m_radius, sphere.m_radius)));
}

bool Sphere3::operator != (const Sphere3& sphere) const
{
    return ((m_center != sphere.m_center) || (!Math::IsEqual(m_radius, sphere.m_radius)));
}
