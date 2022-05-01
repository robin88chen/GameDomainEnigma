#include "Plane3.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

Plane3::Plane3()
{
}

Plane3::Plane3(const Vector3& normal, float constant) :
    m_normal(normal)
{
    m_constant = constant;
}

Plane3::Plane3(const Vector3& normal, const Vector3& p) :
    m_normal(normal)
{
    m_constant = normal.Dot(p);
}

Plane3::Plane3(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
    Vector3 edge1 = p1 - p0;
    Vector3 edge2 = p2 - p0;
    m_normal = edge1.UnitCross(edge2);
    m_constant = m_normal.Dot(p0);
}

bool Plane3::operator == (const Plane3& plane) const
{
    return ((m_normal == plane.m_normal) && (Math::IsEqual(m_constant, plane.m_constant)));
}

bool Plane3::operator != (const Plane3& plane) const
{
    return ((m_normal != plane.m_normal) || (!Math::IsEqual(m_constant, plane.m_constant)));
}

float Plane3::DistanceTo(const Vector3& p) const
{
    return m_normal.Dot(p) - m_constant;
}

Plane3::SideOfPlane Plane3::WhichSide(const Vector3& q) const
{
    float distance = DistanceTo(q);

    if (distance < -Math::EPSILON)
        return SideOfPlane::Negative;

    if (distance > Math::EPSILON)
        return SideOfPlane::Positive;

    return SideOfPlane::Overlap;
}
