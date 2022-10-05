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

bool Ray3::TestIntersectTriangle(Vector3 triangle[3])
{
    Vector3 e1 = triangle[1] - triangle[0];
    Vector3 e2 = triangle[2] - triangle[0];
    Vector3 q = m_direction.Cross(e2);
    float a = e1.Dot(q);
    if ((a > -Math::Epsilon()) && (a < Math::Epsilon())) return false; // parallel
    float f = 1.0f / a;
    Vector3 s = m_origin - triangle[0];
    float u = f * (s.Dot(q));
    if (u < 0.0f || u > 1.0f) return false;
    Vector3 r = s.Cross(e1);
    float v = f * (m_direction.Dot(r));
    if ((v < 0.0f) || (u + v > 1.0f)) return false;
    return true;
}

float Ray3::FindIntersectTriangle(Vector3 triangle[3])
{
    Vector3 e1 = triangle[1] - triangle[0];
    Vector3 e2 = triangle[2] - triangle[0];
    Vector3 q = m_direction.Cross(e2);
    float a = e1.Dot(q);
    if ((a > -Math::Epsilon()) && (a < Math::Epsilon())) return -1.0f;
    float f = 1.0f / a;
    Vector3 s = m_origin - triangle[0];
    float u = f * (s.Dot(q));
    if (u < 0.0f || u > 1.0f) return -1.0f;
    Vector3 r = s.Cross(e1);
    float v = f * (m_direction.Dot(r));
    if ((v < 0.0f) || (u + v > 1.0f)) return -1.0f;

    // if we got here, we found intersection
    float t = f * (e2.Dot(r));
    return t;
}
