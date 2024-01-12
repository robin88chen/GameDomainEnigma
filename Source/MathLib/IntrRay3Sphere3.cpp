#include "IntrRay3Sphere3.h"
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

IntrRay3Sphere3::IntrRay3Sphere3(const Ray3& ray, const Sphere3& sphere) :
    m_ray{ ray }, m_sphere{ sphere }
{
    m_intersectionType = IntersectionType::POINT;
    m_quantity = 0;
}

const Ray3& IntrRay3Sphere3::GetRay() const
{
    return m_ray;
}

const Sphere3& IntrRay3Sphere3::GetSphere() const
{
    return m_sphere;
}

Intersector::Result IntrRay3Sphere3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    /** RaySphere intersection, form Real-time Rendering */
    Vector3 l = m_sphere.Center() - m_ray.origin();
    float s = l.Dot(m_ray.direction());
    float sq_l = l.SquaredLength();
    float sq_r = m_sphere.Radius() * m_sphere.Radius();
    if ((s < 0.0f) && (sq_l > sq_r)) return { false, nullptr };

    float sq_m = sq_l - s * s;
    if (sq_m > sq_r) return { false, nullptr };

    return { true, nullptr };
}

Intersector::Result IntrRay3Sphere3::find(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    /** RaySphere intersection, form Real-time Rendering */
    Vector3 l = m_sphere.Center() - m_ray.origin();
    float s = l.Dot(m_ray.direction());
    float sq_l = l.SquaredLength();
    float sq_r = m_sphere.Radius() * m_sphere.Radius();
    if ((s < 0.0f) && (sq_l > sq_r)) return { false, nullptr };

    float sq_m = sq_l - s * s;
    if (sq_m > sq_r) return { false, nullptr };

    float q = sqrt(sq_r - sq_m);
    if (sq_l > sq_r)
    {
        m_quantity = 2;
        m_tParam[0] = s - q;
        m_tParam[1] = s + q;
        m_point[0] = m_tParam[0] * m_ray.direction() + m_ray.origin();
        m_point[1] = m_tParam[1] * m_ray.direction() + m_ray.origin();
    }
    else
    {
        m_quantity = 1;
        m_tParam[0] = s + q;
        m_point[0] = m_tParam[0] * m_ray.direction() + m_ray.origin();
    }
    return { true, nullptr };
}

int IntrRay3Sphere3::GetQuantity() const
{
    return m_quantity;
}

const Vector3& IntrRay3Sphere3::GetPoint(int i) const
{
    assert((i < m_quantity) && (i >= 0));
    return m_point[i];
}

float IntrRay3Sphere3::GetRayT(int i) const
{
    assert((i < m_quantity) && (i >= 0));
    return m_tParam[i];
}
